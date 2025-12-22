// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCPlayStunMontage.generated.h"

class UAnimMontage;

/**
 * [BehaviorTree Task] 스턴 애니메이션 몽타주를 재생합니다.
 * 스턴 상태일 때 재생되는 애니메이션입니다.
 * 애니메이션이 끝날 때까지 InProgress 상태를 유지합니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCPlayStunMontage : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCPlayStunMontage();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
