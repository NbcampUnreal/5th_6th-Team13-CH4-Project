// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_FCIsStunned.generated.h"

/**
 * [BehaviorTree Decorator] 몬스터가 스턴 상태인지 체크합니다.
 * bIsStunned가 true면 조건 실패 → AI 행동 중단
 */
UCLASS()
class FABRICATION_API UBTD_FCIsStunned : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_FCIsStunned();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};