// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_FindRandomLocation.generated.h"

/**
 * [Instance Data] 랜덤 위치 찾기 Task의 인스턴스 데이터
 * StateTree 파라미터와 바인딩됩니다.
 */
USTRUCT()
struct FSTT_FindRandomLocationInstanceData
{
	GENERATED_BODY()

	// [입력] 랜덤 위치 검색 반경 (cm)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float SearchRadius = 1000.f;

	// [출력] 찾은 랜덤 위치
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector RandomLocation = FVector::ZeroVector;
};

/**
 * [Task] Navigation 기반 랜덤 위치를 찾아 반환합니다.
 * 순찰 중인 AI의 랜덤 이동에 사용됩니다.
 */
USTRUCT(meta = (DisplayName = "FC Find Random Location", Category = "FC|Monster|AI"))
struct FABRICATION_API FSTT_FindRandomLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	// 인스턴스 데이터 타입 연결 (필수)
	using FInstanceDataType = FSTT_FindRandomLocationInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	// 실행 로직
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};