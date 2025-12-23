// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCPlayInvestigateMontage.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Animation/AnimInstance.h"

UBTT_FCPlayInvestigateMontage::UBTT_FCPlayInvestigateMontage()
{
	NodeName = "FC Play Investigate Montage";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_FCPlayInvestigateMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	if (!Monster->InvestigateMontage)
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerComp = &OwnerComp;

	// 수색 애니메이션 재생
	Monster->Multicast_PlayInvestigateAnim();

	// 몽타주 종료 델리게이트 바인딩
	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FOnMontageEnded EndedDelegate;
		EndedDelegate.BindUObject(this, &UBTT_FCPlayInvestigateMontage::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndedDelegate, Monster->InvestigateMontage);
	}

	return EBTNodeResult::InProgress;
}

void UBTT_FCPlayInvestigateMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTT_FCPlayInvestigateMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		CachedOwnerComp.Reset();
		return;
	}

	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster || !Monster->InvestigateMontage)
	{
		CachedOwnerComp.Reset();
		return;
	}

	// [멀티플레이] 모든 클라이언트에서 몽타주 정지
	Monster->Multicast_StopInvestigateAnim();

	CachedOwnerComp.Reset();
}
