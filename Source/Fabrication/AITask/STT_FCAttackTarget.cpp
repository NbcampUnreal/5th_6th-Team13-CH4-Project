// Fill out your copyright notice in the Description page of Project Settings.


#include "AITask/STT_FCAttackTarget.h"

#include "Monster/FCMonsterBase.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_FCAttackTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AFCMonsterBase* Monster = Cast<AFCMonsterBase>(Context.GetOwner());

	if (!Monster) return EStateTreeRunStatus::Failed;

	if (Monster->TryAttackTarget())
	{
		return EStateTreeRunStatus::Succeeded;
	}
    
	return EStateTreeRunStatus::Failed;
}