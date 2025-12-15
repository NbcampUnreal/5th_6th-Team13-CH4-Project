// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckPlayerGaze.generated.h"

/**
 * [BehaviorTree Service] 플레이어 시선 감지 (BlinkHunter 전용)
 * 주기적으로 플레이어가 몬스터를 바라보고 있는지 체크하여 Blackboard 업데이트
 */
UCLASS()
class FABRICATION_API UBTS_CheckPlayerGaze : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_CheckPlayerGaze();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};