// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Decorator/BTD_FCIsStunned.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"

UBTD_FCIsStunned::UBTD_FCIsStunned()
{
	NodeName = "Is NOT Stunned";
}

bool UBTD_FCIsStunned::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return false;
	}

	// Monster 획득
	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster)
	{
		return false;
	}

	// 스턴 상태가 아니면 true (정상 AI 진행)
	return !Monster->bIsStunned;
}