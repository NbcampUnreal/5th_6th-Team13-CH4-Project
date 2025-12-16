// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTS_FCUpdateLure.generated.h"

/**
 * Lure 서비스용 노드 메모리
 * BT 노드는 인스턴스가 공유되므로 NodeMemory를 사용해야 함
 */
USTRUCT()
struct FBTService_UpdateLureMemory
{
	GENERATED_BODY()

	/** Lure가 시작된 시간 */
	float LureStartTime = 0.0f;

	/** Lure 활성화 여부 (이전 Tick 상태) */
	bool bWasLureActive = false;
};

/**
 * [BehaviorTree Service] SoundHunter의 Lure(유인) 상태를 주기적으로 업데이트합니다.
 * - Lure 타임아웃 체크 (LureDuration 경과 시 자동 해제)
 * - Lure 위치 도착 체크
 *
 * 사용법:
 * - SoundHunter의 BehaviorTree Root에 이 서비스를 붙여서 항상 실행되게 함
 * - 블랙보드 키를 에디터에서 설정
 */
UCLASS()
class FABRICATION_API UBTS_FCUpdateLure : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_FCUpdateLure();

	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

#pragma region Blackboard Keys

public:
	/** Lure 활성화 여부를 저장하는 블랙보드 키 (Bool) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasLureTargetKey;

	/** Lure 위치를 저장하는 블랙보드 키 (Vector) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LureLocationKey;

#pragma endregion
};