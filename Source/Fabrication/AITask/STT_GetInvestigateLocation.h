// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_GetInvestigateLocation.generated.h"

/**
 * [Instance Data] 태스크 실행 중 메모리에 저장될 데이터 (Output용)
 * StateTree의 파라미터(블랙보드)와 바인딩됩니다.
 */
USTRUCT()
struct FSTT_GetInvestigateLocationInstanceData
{
	GENERATED_BODY()

	// [출력] 계산된 수색 위치
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector TargetLocation = FVector::ZeroVector;
};

/**
 * [Task] 플레이어가 있을 법한 '수색 위치'를 계산하여 반환합니다.
 * 몬스터의 GetInvestigateLocation 함수를 호출합니다.
 */
USTRUCT(meta = (DisplayName = "FC Get Investigate Location", Category = "FC|Monster|AI"))
struct FABRICATION_API FSTT_GetInvestigateLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	// 인스턴스 데이터 타입 연결 (필수)
	using FInstanceDataType = FSTT_GetInvestigateLocationInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	// 실행 로직
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};