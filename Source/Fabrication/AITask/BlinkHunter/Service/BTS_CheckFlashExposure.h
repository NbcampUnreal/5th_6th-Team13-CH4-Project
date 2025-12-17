// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTS_CheckFlashExposure.generated.h"

// 서비스 노드 메모리 (틱 간 상태 저장용)
struct FBTSCheckFlashExposureMemory
{
	/** 이전 틱에서 스턴 상태였는지 */
	bool bWasPreviouslyStunned = false;
};

/**
 * [BehaviorTree Service] Flash 빛 노출 체크 (BlinkHunter 전용)
 *
 * 기능:
 * 1. Flash 빛 노출 시간 업데이트
 * 2. FlashStunReady 블랙보드 갱신 (노출 시간 >= 임계값)
 * 3. IsStunned 블랙보드 갱신 (몬스터 상태 동기화)
 * 4. 스턴 종료 시 (true → false) TargetPlayer 초기화
 */
UCLASS()
class FABRICATION_API UBTS_CheckFlashExposure : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_CheckFlashExposure();

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTSCheckFlashExposureMemory); }
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

#pragma region Blackboard Keys

public:
	/** 스턴 상태를 저장할 블랙보드 키 (Bool) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsStunnedKey;

	/** Flash 스턴 준비 상태를 저장할 블랙보드 키 (Bool) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector FlashStunReadyKey;

	/** 타겟 플레이어를 저장할 블랙보드 키 (Object) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetPlayerKey;

#pragma endregion
};