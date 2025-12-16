// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTS_CheckPlayerGaze.generated.h"

// 서비스 노드 메모리 (틱 간 상태 저장용)
struct FBTSCheckPlayerGazeMemory
{
	/** 이전 틱에서 관찰되고 있었는지 */
	bool bWasPreviouslyWatched = false;

	/** 마지막으로 관찰한 플레이어 (약한 참조) */
	TWeakObjectPtr<class AFCPlayerCharacter> LastWatchingPlayer;
};

/**
 * [BehaviorTree Service] 플레이어 시선 감지 (BlinkHunter 전용)
 *
 * 기능:
 * 1. 플레이어가 몬스터를 바라보고 있는지 체크 → IsBeingWatched 블랙보드 갱신
 * 2. 관찰 중이면 이동 속도 0, 아니면 빠른 속도
 * 3. 관찰이 끝나면 (true → false) 마지막으로 본 플레이어를 TargetPlayer로 설정
 */
UCLASS()
class FABRICATION_API UBTS_CheckPlayerGaze : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_CheckPlayerGaze();

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTSCheckPlayerGazeMemory); }
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

#pragma region Blackboard Keys

public:
	/** 관찰 여부를 저장할 블랙보드 키 (Bool) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsBeingWatchedKey;

	/** 타겟 플레이어를 저장할 블랙보드 키 (Object) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetPlayerKey;

#pragma endregion
};