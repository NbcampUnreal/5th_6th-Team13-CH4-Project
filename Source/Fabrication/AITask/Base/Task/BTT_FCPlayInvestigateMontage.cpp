// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCPlayInvestigateMontage.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"

UBTT_FCPlayInvestigateMontage::UBTT_FCPlayInvestigateMontage()
{
	NodeName = "FC Play Investigate Montage";
}

EBTNodeResult::Type UBTT_FCPlayInvestigateMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// 수색 애니메이션 재생 (멀티캐스트)
	Monster->Multicast_PlayInvestigateAnim();

	return EBTNodeResult::Succeeded;
}
