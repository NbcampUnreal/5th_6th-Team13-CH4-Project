// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_MoveToActor.generated.h"

/**
 * [Instance Data] Actor로 이동 Task의 인스턴스 데이터
 */
USTRUCT()
struct FSTT_MoveToActorInstanceData
{
	GENERATED_BODY()

	// [입력] 목표 Actor
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> TargetActor = nullptr;

	// [입력] 목표 지점 도달 허용 반경 (cm)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AcceptanceRadius = 100.f;

	// [입력] 계속 추적할지 여부 (true면 Actor가 움직여도 따라감)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bStopOnOverlap = false;
};

/**
 * [Task] 지정된 Actor를 향해 이동합니다.
 * StateTreeComponent용 MoveTo 대체 Task입니다.
 */
USTRUCT(meta = (DisplayName = "FC Move To Actor", Category = "FC|Monster|Movement"))
struct FABRICATION_API FSTT_MoveToActor : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	// 인스턴스 데이터 타입 연결 (필수)
	using FInstanceDataType = FSTT_MoveToActorInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	// Task 진입 (이동 시작)
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	// 매 프레임 실행 (이동 완료 체크)
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	// Task 종료 (이동 정지)
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};