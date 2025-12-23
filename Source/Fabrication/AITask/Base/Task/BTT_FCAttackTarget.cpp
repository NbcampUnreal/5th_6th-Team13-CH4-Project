// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCAttackTarget.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"

UBTT_FCAttackTarget::UBTT_FCAttackTarget()
{
	NodeName = "FC Attack Target";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_FCAttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (Monster->bIsStunned || !Monster->bCanAttack)
	{
		return EBTNodeResult::Failed;
	}

	if (!Monster->AttackMontage)
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerComp = &OwnerComp;

	// 공격 수행 (데미지 + 애니메이션)
	Monster->PerformMeleeAttack();
	Monster->bCanAttack = false;

	// 몽타주 종료 델리게이트 바인딩
	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FOnMontageEnded EndedDelegate;
		EndedDelegate.BindUObject(this, &UBTT_FCAttackTarget::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndedDelegate, Monster->AttackMontage);
	}

	return EBTNodeResult::InProgress;
}

void UBTT_FCAttackTarget::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTT_FCAttackTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		CachedOwnerComp.Reset();
		return;
	}

	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster || !Monster->AttackMontage)
	{
		CachedOwnerComp.Reset();
		return;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(Monster->AttackMontage))
	{
		AnimInstance->Montage_Stop(0.2f, Monster->AttackMontage);
	}

	CachedOwnerComp.Reset();
}
