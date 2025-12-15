// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_FCHasTarget.generated.h"

/**
 * [BehaviorTree Decorator] Blackboard의 TargetPlayer가 유효한지 체크합니다.
 */
UCLASS()
class FABRICATION_API UBTD_FCHasTarget : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_FCHasTarget();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	/** Blackboard Key Selector */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetPlayerKey;
};