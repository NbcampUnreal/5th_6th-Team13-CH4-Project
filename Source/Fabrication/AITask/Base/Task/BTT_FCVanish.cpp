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
	if (Monster->IsHidden())
	{
		return EBTNodeResult::Succeeded;
	}

	// 1. 모습 숨기기 & 충돌 끄기
	Monster->SetActorHiddenInGame(true);
	Monster->SetActorEnableCollision(false);

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