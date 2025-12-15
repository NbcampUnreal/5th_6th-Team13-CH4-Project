// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckFlashExposure.generated.h"

/**
 * [BehaviorTree Service] Flash 빛 노출 체크 (BlinkHunter 전용)
 * 주기적으로 Flash 빛에 노출되었는지 체크하고, 3초 누적 시 스턴 적용
 */
UCLASS()
class FABRICATION_API UBTS_CheckFlashExposure : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_CheckFlashExposure();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};