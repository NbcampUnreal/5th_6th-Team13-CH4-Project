// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BTS_CheckFlashExposure.h"
#include "Monster/FCMonsterBlinkHunter.h"
#include "MonsterController/FCMonsterAIController.h"

UBTS_CheckFlashExposure::UBTS_CheckFlashExposure()
{
	NodeName = "Check Flash Exposure";
	Interval = 0.1f; // 0.1초마다 체크
	RandomDeviation = 0.0f;
}

void UBTS_CheckFlashExposure::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return;
	}

	// BlinkHunter 획득
	AFCMonsterBlinkHunter* Monster = Cast<AFCMonsterBlinkHunter>(AICon->GetMonster());
	if (!Monster)
	{
		return;
	}

	// 스턴 상태면 체크 안 함
	if (Monster->bIsStunned)
	{
		return;
	}

	// Flash 빛 노출 체크 및 스턴 처리
	Monster->CheckFlashLightExposure(DeltaSeconds);
}