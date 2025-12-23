// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCPlayStunMontage.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Animation/AnimInstance.h"

UBTT_FCPlayStunMontage::UBTT_FCPlayStunMontage()
{
	NodeName = "FC Play Stun Montage";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_FCPlayStunMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (!Monster->StunMontage)
	{
		// StunMontage가 없으면 바로 성공 반환 (스턴은 적용되지만 애니메이션만 없음)
		UE_LOG(LogTemp, Warning, TEXT("[%s] StunMontage가 설정되지 않음"), *Monster->GetName());
		return EBTNodeResult::Succeeded;
	}

	CachedOwnerComp = &OwnerComp;

	// 스턴 애니메이션 재생
	Monster->Multicast_PlayStunAnim();

	UE_LOG(LogTemp, Log, TEXT("[%s] 스턴 몽타주 재생 시작"), *Monster->GetName());

	// 몽타주 종료 델리게이트 바인딩
	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FOnMontageEnded EndedDelegate;
		EndedDelegate.BindUObject(this, &UBTT_FCPlayStunMontage::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndedDelegate, Monster->StunMontage);
	}

	return EBTNodeResult::InProgress;
}

void UBTT_FCPlayStunMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CachedOwnerComp.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("스턴 몽타주 종료 (Interrupted: %s)"), bInterrupted ? TEXT("Yes") : TEXT("No"));
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTT_FCPlayStunMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		CachedOwnerComp.Reset();
		return;
	}

	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster || !Monster->StunMontage)
	{
		CachedOwnerComp.Reset();
		return;
	}

	// Task가 중단되었을 때 몽타주도 정지
	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(Monster->StunMontage))
	{
		AnimInstance->Montage_Stop(0.2f, Monster->StunMontage);
	}

	CachedOwnerComp.Reset();
}
