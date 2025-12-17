// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTS_FCUpdateChaseLocation.generated.h"

/**
 * 추격 중 타겟 위치를 주기적으로 갱신하는 서비스
 *
 * [동작 원리]
 * - SeenPlayer가 있을 때만 LastStimulusLocation을 갱신
 * - SeenPlayer가 없으면 (시야에서 사라지면) 위치 갱신을 중단
 * - 결과적으로 "마지막으로 본 위치"까지만 추격하게 됨
 *
 * [사용법]
 * - Chase Sequence에 이 서비스를 붙임
 * - MoveTo Task의 타겟을 TargetPlayer(Object) 대신 LastStimulusLocation(Vector)으로 설정
 */
UCLASS()
class FABRICATION_API UBTS_FCUpdateChaseLocation : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_FCUpdateChaseLocation();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

protected:
	/** 현재 눈으로 보고 있는 플레이어 (Perception에서 갱신) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SeenPlayerKey;

	/** 추격 대상 플레이어 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetPlayerKey;

	/** 갱신할 위치 키 (MoveTo에서 사용) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastStimulusLocationKey;
};
