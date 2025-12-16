// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTS_FCCheckPlayerDistance.generated.h"

/**
 * [BehaviorTree Service] 주기적으로 플레이어와의 거리를 계산하여 블랙보드 Bool 값을 갱신합니다.
 *
 * 사용 예시:
 * - 공격 거리 체크: bUseMonsterAttackRange=true, bCheckAnyPlayer=false
 * - 탐지 범위 체크: bUseMonsterAttackRange=false, CustomCheckRange=1000, bCheckAnyPlayer=true
 * - 경계 범위 체크: bUseMonsterAttackRange=false, CustomCheckRange=500, bCheckAnyPlayer=true
 */
UCLASS()
class FABRICATION_API UBTS_FCCheckPlayerDistance : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_FCCheckPlayerDistance();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

#pragma region Configuration

public:
	/** true면 몬스터의 AttackRange 사용, false면 CustomCheckRange 사용 */
	UPROPERTY(EditAnywhere, Category = "Distance Check")
	bool bUseMonsterAttackRange = true;

	/** bUseMonsterAttackRange가 false일 때 사용할 커스텀 거리 (cm) */
	UPROPERTY(EditAnywhere, Category = "Distance Check", meta = (EditCondition = "!bUseMonsterAttackRange"))
	float CustomCheckRange = 500.0f;

	/** true면 모든 플레이어 중 가장 가까운 플레이어 체크, false면 TargetPlayer만 체크 */
	UPROPERTY(EditAnywhere, Category = "Distance Check")
	bool bCheckAnyPlayer = false;

	/** 타겟 플레이어 키 (Object 타입) - bCheckAnyPlayer가 false일 때 사용 */
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "!bCheckAnyPlayer"))
	FBlackboardKeySelector TargetPlayerKey;

	/** 결과를 저장할 블랙보드 키 (Bool 타입) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector OutputKey;

#pragma endregion
};
