// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BTT_FCAttackTarget.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FCAttackTarget::UBTT_FCAttackTarget()
{
	NodeName = "FC Attack Target";
}

EBTNodeResult::Type UBTT_FCAttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// [멀티플레이] BehaviorTree는 서버에서만 실행되므로 별도의 Authority 체크 불필요

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

	// 공격 시도 (Monster 내부에서 HasAuthority() 체크함)
	if (Monster->TryAttackTarget())
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}