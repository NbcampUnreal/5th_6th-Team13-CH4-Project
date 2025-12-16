// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTT_FCApplyFlashStun.generated.h"

/**
 * [BehaviorTree Task] Flash 스턴을 적용하고 노출 시간을 리셋합니다.
 * BlinkHunter 전용 - Flash 빛에 일정 시간 노출 시 스턴 상태로 전환
 */
UCLASS()
class FABRICATION_API UBTT_FCApplyFlashStun : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCApplyFlashStun();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsStunnedKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector FlashStunReadyKey;
};
