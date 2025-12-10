// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_MoveToLocation.generated.h"

/**
 * [Instance Data] 위치로 이동 Task의 인스턴스 데이터
 */
USTRUCT()
struct FSTT_MoveToLocationInstanceData
{
	GENERATED_BODY()

	// [입력] 목표 위치
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector TargetLocation = FVector::ZeroVector;

	// [입력] 목표 지점 도달 허용 반경 (cm)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AcceptanceRadius = 100.f;
};

/**
 * [Task] 지정된 위치로 이동합니다.
 * StateTreeComponent용 MoveTo 대체 Task입니다.
 */
USTRUCT(meta = (DisplayName = "FC Move To Location", Category = "FC|Monster|Movement"))
struct FABRICATION_API FSTT_MoveToLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	// 인스턴스 데이터 타입 연결 (필수)
	using FInstanceDataType = FSTT_MoveToLocationInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	// Task 진입 (이동 시작)
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	// 매 프레임 실행 (이동 완료 체크)
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	// Task 종료 (이동 정지)
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};