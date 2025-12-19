// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCAttackTarget.generated.h"

class UAnimMontage;

/**
 * [BehaviorTree Task] 사거리 내에 타겟이 있으면 공격을 시도합니다.
 * 공격 애니메이션이 끝날 때까지 InProgress 상태를 유지합니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCAttackTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCAttackTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
