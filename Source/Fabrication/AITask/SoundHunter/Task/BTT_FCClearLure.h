// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTT_FCClearLure.generated.h"

/**
 * [BehaviorTree Task] SoundHunter의 Lure(유인) 타겟을 해제합니다.
 * - Monster와 Blackboard의 Lure 상태를 초기화
 * - Lure 위치 도착 시 또는 타임아웃 후 호출
 *
 * 사용 예시:
 * - Lure 위치 도착 후 상태 초기화
 * - Lure 추적 실패 시 상태 초기화
 */
UCLASS()
class FABRICATION_API UBTT_FCClearLure : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCClearLure();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	/** Lure 활성화 여부를 저장하는 블랙보드 키 (Bool) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasLureTargetKey;

	/** Lure 위치를 저장하는 블랙보드 키 (Vector) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LureLocationKey;
};