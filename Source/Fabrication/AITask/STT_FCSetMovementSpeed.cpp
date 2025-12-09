// Fill out your copyright notice in the Description page of Project Settings.


#include "AITask/STT_FCSetMovementSpeed.h"

#include "Monster/FCMonsterBase.h" 
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_FCSetMovementSpeed::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// GetOwner()는 UObject*를 반환하므로 바로 Cast
	AFCMonsterBase* Monster = Cast<AFCMonsterBase>(Context.GetOwner());

	if (!Monster)
	{
		return EStateTreeRunStatus::Failed;
	}

	Monster->SetMovementSpeed(NewSpeed);
	return EStateTreeRunStatus::Succeeded;
}