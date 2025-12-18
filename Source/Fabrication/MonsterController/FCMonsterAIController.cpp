// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterController/FCMonsterAIController.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Fabrication.h"

AFCMonsterAIController::AFCMonsterAIController()
{
	// 팀 ID 설정 (0: No Team, 1: Survivor, 2: Monster 라고 가정)
	MonsterTeamID = FGenericTeamId(2);

	// [Blackboard Component] : 실제 실행 중 데이터를 저장하는 "기억장치"
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// [멀티플레이] AIController는 복제되지 않음 (서버에만 존재)
	bReplicates = false;

	// [멀티플레이] AIController의 네트워크 업데이트 빈도 (복제되지 않으므로 의미 없지만 명시적 설정)
	SetNetUpdateFrequency(1.0f);

	// [멀티플레이] AI 감각 시스템 생성 (서버에서만 실행됨)
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// 시야(Sight) 설정 생성 (실제 값은 OnPossess에서 ApplySightConfig를 통해 적용)
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// 소속 감지 설정 (모두 감지)
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
}

AFCMonsterBase* AFCMonsterAIController::GetMonster() const
{
	return Cast<AFCMonsterBase>(GetPawn());
}

void AFCMonsterAIController::RestartBehaviorTree()
{
	// BehaviorTree 재시작
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AFCMonsterAIController::StopBehaviorTree()
{
	// BehaviorTree 정지
	if (UBrainComponent* BrainComp = GetBrainComponent())
	{
		BrainComp->StopLogic(TEXT("Stop"));
	}
}

void AFCMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// [멀티플레이] AI는 서버에서만 실행됨을 보장
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("AFCMonsterAIController::OnPossess called on client! AI should only run on server."));
		return;
	}

	// Sight 설정 적용 (블루프린트에서 설정한 값으로)
	ApplySightConfig();

	// Perception 델리게이트 바인딩 (서버에서만)
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AFCMonsterAIController::OnTargetPerceptionUpdated);
	}

	// BehaviorTree 실행 (블루프린트에서 할당된 경우)
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AFCMonsterAIController::OnUnPossess()
{
	// BehaviorTree 정지
	if (UBrainComponent* BrainComp = GetBrainComponent())
	{
		BrainComp->StopLogic(TEXT("UnPossess"));
	}

	Super::OnUnPossess();
}

FGenericTeamId AFCMonsterAIController::GetGenericTeamId() const
{
	return MonsterTeamID;
}

void AFCMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// [멀티플레이] AIController는 서버 전용이므로 여기는 서버에서만 실행됨
	// 명시적으로 체크 (안전성)
	if (!HasAuthority()) return;

	// Player만 처리
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
	if (!Player) return;

	// Monster 획득
	AFCMonsterBase* Monster = GetMonster();
	if (!Monster) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		// [발견] 시야에 들어옴
		// 1. Monster의 Replicated 변수 업데이트 (클라이언트에서 애니메이션/이펙트용)
		Monster->SeenPlayer = Player;
		Monster->TargetPlayer = Player;
		Monster->LastStimulusLocation = Stimulus.StimulusLocation;

		// 2. Blackboard 업데이트 (BehaviorTree가 사용, 서버 전용)
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsObject(TEXT("TargetPlayer"), Player);
			BlackboardComp->SetValueAsObject(TEXT("SeenPlayer"), Player);
			BlackboardComp->SetValueAsVector(TEXT("LastStimulusLocation"), Stimulus.StimulusLocation);
		}

		UE_LOG(LogTemp, Log, TEXT("[AI] Monster detected Player: %s"), *Player->GetName());
	}
	else
	{
		// [놓침] 시야에서 사라짐
		// 바로 포기하지 않고, 마지막 위치까지는 수색
		if (Monster->SeenPlayer == Player)
		{
			// 1. Monster의 Replicated 변수 업데이트
			Monster->SeenPlayer = nullptr;
			// TargetPlayer는 유지 (BehaviorTree에서 마지막 위치 수색 후 초기화)

			// 2. Blackboard 업데이트
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsObject(TEXT("SeenPlayer"), nullptr);
				// TargetPlayer와 LastStimulusLocation은 유지
			}

			UE_LOG(LogTemp, Log, TEXT("[AI] Monster lost sight of Player: %s"), *Player->GetName());
		}
	}
}

void AFCMonsterAIController::ApplySightConfig()
{
	if (!SightConfig || !AIPerceptionComponent) return;

	// Monster의 DataTable 데이터 확인
	AFCMonsterBase* Monster = GetMonster();
	if (Monster && Monster->bDataTableLoaded)
	{
		// DataTable에서 로드된 값 사용
		const FFCMonsterDataRow& Data = Monster->CachedMonsterData;
		SightRadius = Data.SightRadius;
		LoseSightRadius = Data.LoseSightRadius;
		PeripheralVisionAngleDegrees = Data.PeripheralVisionAngleDegrees;
		bSightEnabled = Data.bSightEnabled;

		FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Perception 값을 DataTable에서 로드"), *GetName());
	}

	// Sight 설정 적용
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;

	// Perception 컴포넌트에 설정 적용
	AIPerceptionComponent->ConfigureSense(*SightConfig);

	// 시야가 활성화된 경우에만 Dominant Sense로 설정
	if (bSightEnabled)
	{
		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Sight Config 적용 - Radius: %.0f, LoseRadius: %.0f, Angle: %.0f, Enabled: %s"),
		*GetName(), SightRadius, LoseSightRadius, PeripheralVisionAngleDegrees, bSightEnabled ? TEXT("true") : TEXT("false"));
}
