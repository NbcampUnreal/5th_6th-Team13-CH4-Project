// Fill out your copyright notice in the Description page of Project Settings.


#include "AITask/STT_FCSetMovementSpeed.h"

#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_FCSetMovementSpeed::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 인스턴스 데이터 가져오기
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	// Owner는 AIController -> GetPawn()으로 Monster 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(Context.GetOwner());
	if (!AICon) return EStateTreeRunStatus::Failed;

	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster) return EStateTreeRunStatus::Failed;

	Monster->SetMovementSpeed(InstanceData.NewSpeed);
	return EStateTreeRunStatus::Succeeded;
}