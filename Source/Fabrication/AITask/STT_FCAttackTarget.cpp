// Fill out your copyright notice in the Description page of Project Settings.


#include "AITask/STT_FCAttackTarget.h"

#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_FCAttackTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// Owner는 AIController -> GetMonster()로 Monster 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(Context.GetOwner());
	if (!AICon) return EStateTreeRunStatus::Failed;

	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster) return EStateTreeRunStatus::Failed;

	if (Monster->TryAttackTarget())
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}