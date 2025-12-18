// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTT_FCClearHeardSound.generated.h"

/**
 * [BehaviorTree Task] SoundHunter의 HeardSound(일반 소리) 상태를 해제합니다.
 * - Monster와 Blackboard의 HeardSound 상태를 초기화
 * - 소리 위치 도착 후 조사 완료 시 호출
 *
 * 사용 예시:
 * - 소리 위치 도착 후 상태 초기화
 * - 소리 추적 실패 시 상태 초기화
 */
UCLASS()
class FABRICATION_API UBTT_FCClearHeardSound : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCClearHeardSound();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	/** 소리 감지 여부를 저장하는 블랙보드 키 (Bool) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasHeardSoundKey;

	/** 소리 위치를 저장하는 블랙보드 키 (Vector) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastHeardLocationKey;
};
