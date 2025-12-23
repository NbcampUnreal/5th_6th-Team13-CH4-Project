// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Service/BTS_FCUpdateChaseLocation.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Monster/FCMonsterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_FCUpdateChaseLocation::UBTS_FCUpdateChaseLocation()
{
	NodeName = "Update Chase Location";

	// [최적화] 매 프레임이 아니라 0.15초마다 실행 (타이머 역할)
	Interval = 0.15f;
	RandomDeviation = 0.05f; // 여러 몬스터가 동시에 연산하지 않도록 분산

	// 블랙보드 키 필터 설정
	SeenPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_FCUpdateChaseLocation, SeenPlayerKey), AActor::StaticClass());
	TargetPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_FCUpdateChaseLocation, TargetPlayerKey), AActor::StaticClass());
	LastStimulusLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_FCUpdateChaseLocation, LastStimulusLocationKey));
}

void UBTS_FCUpdateChaseLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	// 블랙보드 에셋에서 키 정보 해결
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		SeenPlayerKey.ResolveSelectedKey(*BBAsset);
		TargetPlayerKey.ResolveSelectedKey(*BBAsset);
		LastStimulusLocationKey.ResolveSelectedKey(*BBAsset);

		// [유효성 검증] 필수 Blackboard 키가 설정되었는지 확인
		if (!SeenPlayerKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_FCUpdateChaseLocation] SeenPlayerKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
		if (!TargetPlayerKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_FCUpdateChaseLocation] TargetPlayerKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
		if (!LastStimulusLocationKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_FCUpdateChaseLocation] LastStimulusLocationKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
	}
}

void UBTS_FCUpdateChaseLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AFCMonsterAIController* AIController = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	AFCMonsterBase* Monster = AIController ? AIController->GetMonster() : nullptr;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Monster || !Blackboard)
	{
		return;
	}

	// 현재 "눈으로 보고 있는" 플레이어 (Perception에서 갱신됨)
	AActor* SeenActor = Cast<AActor>(Blackboard->GetValueAsObject(SeenPlayerKey.SelectedKeyName));

	// 현재 추격 대상
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetPlayerKey.SelectedKeyName));

	// [핵심 로직]
	// SeenPlayer가 있고, 추격 대상과 동일할 때만 위치를 갱신
	// 벽 뒤로 숨어서 SeenPlayer가 nullptr이 되면 위치 갱신이 중단됨
	// → LastStimulusLocation은 "마지막으로 본 위치"에 고정됨
	if (SeenActor && SeenActor == TargetActor)
	{
		Blackboard->SetValueAsVector(LastStimulusLocationKey.SelectedKeyName, SeenActor->GetActorLocation());
	}
}

FString UBTS_FCUpdateChaseLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("SeenPlayer 존재 시 %s 위치를 %s에 갱신\n시야에서 사라지면 마지막 위치 유지"),
		*TargetPlayerKey.SelectedKeyName.ToString(),
		*LastStimulusLocationKey.SelectedKeyName.ToString());
}
