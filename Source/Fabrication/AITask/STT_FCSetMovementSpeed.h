// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_FCSetMovementSpeed.generated.h"

/**
 * [Task] 몬스터의 이동 속도를 변경합니다.
 * StateTree에서 추격/순찰 상태 전환 시 호출됩니다.
 */
USTRUCT(meta = (DisplayName = "FC Set Movement Speed", Category = "FC|Monster|Action"))
struct FABRICATION_API FSTT_FCSetMovementSpeed : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	// [입력] 변경할 이동 속도 (cm/s)
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float NewSpeed = 400.f;

	// Task 진입 시 1회 실행
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};