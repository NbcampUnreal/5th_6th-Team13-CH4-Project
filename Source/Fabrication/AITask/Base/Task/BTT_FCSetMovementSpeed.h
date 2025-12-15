// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCSetMovementSpeed.generated.h"

/**
 * [BehaviorTree Task] 몬스터의 이동 속도를 변경합니다.
 * BehaviorTree에서 추격/순찰 상태 전환 시 호출됩니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCSetMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCSetMovementSpeed();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 변경할 이동 속도 (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float NewSpeed = 400.0f;
};