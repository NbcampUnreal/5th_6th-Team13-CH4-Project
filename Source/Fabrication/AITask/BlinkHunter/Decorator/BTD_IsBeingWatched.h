// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsBeingWatched.generated.h"

/**
 * [BehaviorTree Decorator] 플레이어가 몬스터를 관찰하고 있는지 체크 (BlinkHunter 전용)
 * Blackboard의 "IsBeingWatched" 키를 확인
 */
UCLASS()
class FABRICATION_API UBTD_IsBeingWatched : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_IsBeingWatched();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};