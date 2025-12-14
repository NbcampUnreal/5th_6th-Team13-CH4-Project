// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BTT_FCVanish.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"

UBTT_FCVanish::UBTT_FCVanish()
{
	NodeName = "FC Vanish";
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

	// 2. 타겟 초기화
	Monster->TargetPlayer = nullptr;
	Monster->SeenPlayer = nullptr;

	return EBTNodeResult::Succeeded;
}