// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterController/FCSoundHunterAIController.h"
#include "Monster/FCSoundHunter.h"
#include "Player/FCPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

AFCSoundHunterAIController::AFCSoundHunterAIController()
{
	// 청력(Hearing) 설정 생성 (실제 값은 OnPossess에서 ApplyHearingConfig를 통해 적용)
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;   // 아이템(Lure) 감지
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
}

AFCSoundHunter* AFCSoundHunterAIController::GetSoundHunter() const
{
	return Cast<AFCSoundHunter>(GetPawn());
}

void AFCSoundHunterAIController::OnPossess(APawn* InPawn)
{
	// 부모의 델리게이트 바인딩 전에 Clear (부모가 Super에서 바인딩함)
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.Clear();
	}

	// 부모 OnPossess 호출 (BehaviorTree 실행 등)
	Super::OnPossess(InPawn);

	// [멀티플레이] 서버에서만 실행
	if (!HasAuthority()) return;

	// Hearing 설정 적용 (블루프린트에서 설정한 값으로)
	ApplyHearingConfig();

	// 새로운 Perception 델리게이트 바인딩 (Hearing 처리 포함)
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
			this, &AFCSoundHunterAIController::OnPerceptionUpdated_SoundHunter);
	}
}

void AFCSoundHunterAIController::OnPerceptionUpdated_SoundHunter(AActor* Actor, FAIStimulus Stimulus)
{
	// [멀티플레이] 서버 전용
	if (!HasAuthority()) return;

	// Hearing 자극 처리
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		HandleHearingStimulus(Actor, Stimulus);
		return;  // Hearing은 여기서 처리 완료
	}

	// Sight 자극 처리 (부모 로직 재사용)
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		HandlePerceptionUpdated(Actor, Stimulus);
	}
}

void AFCSoundHunterAIController::HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 부모의 Sight 처리 로직 재구현 (부모 함수가 private이므로)
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
	if (!Player) return;

	AFCSoundHunter* Monster = GetSoundHunter();
	if (!Monster) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		// [발견] 시야에 들어옴
		Monster->SeenPlayer = Player;
		Monster->TargetPlayer = Player;
		Monster->LastStimulusLocation = Stimulus.StimulusLocation;

		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsObject(TEXT("TargetPlayer"), Player);
			BlackboardComp->SetValueAsObject(TEXT("SeenPlayer"), Player);
			BlackboardComp->SetValueAsVector(TEXT("LastStimulusLocation"), Stimulus.StimulusLocation);
		}

		UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Sight - Detected Player: %s"), *Player->GetName());
	}
	else
	{
		// [놓침] 시야에서 사라짐
		if (Monster->SeenPlayer == Player)
		{
			Monster->SeenPlayer = nullptr;

			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsObject(TEXT("SeenPlayer"), nullptr);
			}

			UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Sight - Lost Player: %s"), *Player->GetName());
		}
	}
}

void AFCSoundHunterAIController::HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
	AFCSoundHunter* Monster = GetSoundHunter();
	if (!Monster) return;

	// 소리 감지 실패 시 무시
	if (!Stimulus.WasSuccessfullySensed()) return;

	const FVector SoundLocation = Stimulus.StimulusLocation;

	// "Lure" 태그 체크 (유인 아이템) - 무조건 최우선 반응!
	if (Stimulus.Tag == FName("Lure"))
	{
		// Lure는 최우선! 현재 타겟 무시하고 Lure 위치로
		Monster->SetLureTarget(SoundLocation);

		// 기존 타겟 초기화 (Lure에 집중)
		Monster->TargetPlayer = nullptr;

		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool(TEXT("bHasLureTarget"), true);
			BlackboardComp->SetValueAsVector(TEXT("LureLocation"), SoundLocation);
			BlackboardComp->ClearValue(TEXT("TargetPlayer"));
		}

		UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Hearing - LURE detected at: %s"), *SoundLocation.ToString());
		return;
	}

	// === 일반 소리: 이미 타겟이 있거나 Lure 추적 중이면 무시 ===
	if (Monster->TargetPlayer != nullptr || Monster->bHasLureTarget)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Hearing - Ignored (already has target)"));
		return;
	}

	// 타겟이 없을 때만 새 소리에 반응
	Monster->LastHeardLocation = SoundLocation;

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector(TEXT("LastHeardLocation"), SoundLocation);
	}

	// 플레이어가 소리를 냈으면 타겟으로 설정
	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor))
	{
		Monster->TargetPlayer = Player;
		Monster->LastStimulusLocation = SoundLocation;

		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsObject(TEXT("TargetPlayer"), Player);
			BlackboardComp->SetValueAsVector(TEXT("LastStimulusLocation"), SoundLocation);
		}

		UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Hearing - New target Player at: %s"), *SoundLocation.ToString());
	}
	else
	{
		// 플레이어가 아닌 소리 (환경음 등) - 위치만 기록
		UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Hearing - Sound at: %s (not player)"), *SoundLocation.ToString());
	}
}

void AFCSoundHunterAIController::ApplyHearingConfig()
{
	if (!HearingConfig || !AIPerceptionComponent) return;

	// 블루프린트에서 설정한 값으로 Hearing 설정 적용
	HearingConfig->HearingRange = HearingRadius;

	// Perception 컴포넌트에 설정 적용
	AIPerceptionComponent->ConfigureSense(*HearingConfig);

	// 청력이 활성화된 경우에만 Dominant Sense로 설정 (SoundHunter는 청력이 더 중요)
	if (bHearingEnabled)
	{
		AIPerceptionComponent->SetDominantSense(HearingConfig->GetSenseImplementation());
	}

	UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Hearing Config Applied - Radius: %.0f, Enabled: %s"),
		HearingRadius, bHearingEnabled ? TEXT("true") : TEXT("false"));
}