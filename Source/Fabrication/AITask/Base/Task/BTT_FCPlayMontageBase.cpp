// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCPlayMontageBase.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Animation/AnimInstance.h"
#include "Fabrication.h"

UBTT_FCPlayMontageBase::UBTT_FCPlayMontageBase()
{
	// 인스턴스 생성 필수 (Latent Task이므로)
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_FCPlayMontageBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// 몽타주 획득 (자식 클래스에서 구현)
	UAnimMontage* Montage = GetMontage(Monster);
	if (!Montage)
	{
		// 몽타주가 없을 때 동작 (자식 클래스에서 정의)
		return GetNullMontageBehavior();
	}

	// OwnerComp 캐싱 (Latent Task 완료용)
	CachedOwnerComp = &OwnerComp;

	// 몽타주 재생 전 AI Movement 정지 (미끄러짐 방지)
	AICon->StopMovement();

	// 몽타주 재생 (자식 클래스에서 구현)
	PlayMontage(Monster);

	// 몽타주 종료 델리게이트 바인딩
	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FOnMontageEnded EndedDelegate;
		EndedDelegate.BindUObject(this, &UBTT_FCPlayMontageBase::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndedDelegate, Montage);
	}

	return EBTNodeResult::InProgress;
}

void UBTT_FCPlayMontageBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTT_FCPlayMontageBase::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task 중단 시 몽타주 즉시 정지
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (AICon)
	{
		AFCMonsterBase* Monster = AICon->GetMonster();
		if (Monster)
		{
			StopMontage(Monster);
		}
	}

	CachedOwnerComp.Reset();
	return EBTNodeResult::Aborted;
}

void UBTT_FCPlayMontageBase::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		CachedOwnerComp.Reset();
		return;
	}

	// Monster 획득
	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster)
	{
		CachedOwnerComp.Reset();
		return;
	}

	// 몽타주 획득
	UAnimMontage* Montage = GetMontage(Monster);
	if (!Montage)
	{
		CachedOwnerComp.Reset();
		return;
	}

	// 몽타주 정지 (자식 클래스에서 구현)
	StopMontage(Monster);

	CachedOwnerComp.Reset();
}
