// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTT_FCGetInvestigateLocation.generated.h"

/**
 * [BehaviorTree Task] 플레이어가 있을 법한 '수색 위치'를 계산하여 블랙보드에 저장합니다.
 * 몬스터의 GetInvestigateLocation 함수를 호출합니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCGetInvestigateLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCGetInvestigateLocation();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 타겟 플레이어 키 (Object 타입)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetPlayerKey;

	// 결과를 저장할 블랙보드 키 (Vector 타입)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;
};