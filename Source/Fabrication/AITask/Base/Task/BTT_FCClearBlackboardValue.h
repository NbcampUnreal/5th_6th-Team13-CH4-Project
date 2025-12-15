// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCClearBlackboardValue.generated.h"

/**
 * [BehaviorTree Task] 블랙보드의 특정 키 값을 초기화(Clear)합니다.
 * 모든 타입(Object, Vector, Bool, Float, Int 등)을 지원하는 범용 Task입니다.
 *
 * 사용 예시:
 * - 추적 실패 시 TargetPlayer 초기화
 * - 순찰 시작 시 TargetLocation 초기화
 * - 상태 전환 시 LastStimulusLocation 초기화
 */
UCLASS()
class FABRICATION_API UBTT_FCClearBlackboardValue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCClearBlackboardValue();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/**
	 * 초기화할 블랙보드 키
	 * 타입에 상관없이 모든 키를 초기화할 수 있습니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector BlackboardKey;
};