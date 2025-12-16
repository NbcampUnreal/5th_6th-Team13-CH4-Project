// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_FCInAttackRange.generated.h"

/**
 * [BehaviorTree Decorator] 타겟이 공격 범위 내에 있는지 체크합니다.
 * true: 공격 범위 내 (AttackRange 이하)
 * false: 공격 범위 밖
 */
UCLASS()
class FABRICATION_API UBTD_FCInAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_FCInAttackRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
