// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCSetMovementSpeed.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"

UBTT_FCSetMovementSpeed::UBTT_FCSetMovementSpeed()
{
	NodeName = "FC Set Movement Speed";
}

EBTNodeResult::Type UBTT_FCSetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// 이동 속도 변경 (CharacterMovementComponent가 자동으로 복제됨)
	Monster->SetMovementSpeed(NewSpeed);
	return EBTNodeResult::Succeeded;
}