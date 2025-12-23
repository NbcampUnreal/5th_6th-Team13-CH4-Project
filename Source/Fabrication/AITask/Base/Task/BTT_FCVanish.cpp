// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCVanish.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FCVanish::UBTT_FCVanish()
{
	NodeName = "FC Vanish";

	TargetPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCVanish, TargetPlayerKey), AActor::StaticClass());
	SeenPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCVanish, SeenPlayerKey), AActor::StaticClass());
	LastStimulusLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCVanish, LastStimulusLocationKey));
}

void UBTT_FCVanish::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetPlayerKey.ResolveSelectedKey(*BBAsset);
		SeenPlayerKey.ResolveSelectedKey(*BBAsset);
		LastStimulusLocationKey.ResolveSelectedKey(*BBAsset);

		// [유효성 검증] 필수 Blackboard 키가 설정되었는지 확인
		if (!TargetPlayerKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTT_FCVanish] TargetPlayerKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
		if (!SeenPlayerKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTT_FCVanish] SeenPlayerKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
		if (!LastStimulusLocationKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTT_FCVanish] LastStimulusLocationKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
	}
}

EBTNodeResult::Type UBTT_FCVanish::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	// Monster 획득
	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	// 이미 사라진 상태면 패스
	if (Monster->bIsVanished)
	{
		return EBTNodeResult::Succeeded;
	}

	// 1. 모습 숨기기 & 충돌 끄기
	Monster->SetActorHiddenInGame(true);
	Monster->SetActorEnableCollision(false);
	Monster->bIsVanished = true;

	// 2. 타겟 초기화 (Monster의 Replicated 변수)
	Monster->TargetPlayer = nullptr;
	Monster->SeenPlayer = nullptr;

	// 3. [버그 수정] Blackboard 초기화 - BehaviorTree가 null 타겟 참조 방지
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetPlayerKey.SelectedKeyName, nullptr);
		BlackboardComp->SetValueAsObject(SeenPlayerKey.SelectedKeyName, nullptr);
		BlackboardComp->ClearValue(LastStimulusLocationKey.SelectedKeyName);
	}

	return EBTNodeResult::Succeeded;
}