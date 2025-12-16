// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCAttackTarget.generated.h"

/**
 * [BehaviorTree Task] 사거리 내에 타겟이 있으면 공격을 시도합니다.
 * 성공 시 Succeeded, 사거리가 안 닿거나 쿨타임이면 Failed를 반환합니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCAttackTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCAttackTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};