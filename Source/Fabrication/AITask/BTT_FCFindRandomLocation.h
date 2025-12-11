// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCFindRandomLocation.generated.h"

/**
 * [BehaviorTree Task] Navigation 기반 랜덤 위치를 찾아 블랙보드에 저장합니다.
 * 순찰 중인 AI의 랜덤 이동에 사용됩니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCFindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCFindRandomLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 랜덤 위치 검색 반경 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SearchRadius = 1000.0f;

	// 결과를 저장할 블랙보드 키 (Vector 타입)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector RandomLocationKey;
};
