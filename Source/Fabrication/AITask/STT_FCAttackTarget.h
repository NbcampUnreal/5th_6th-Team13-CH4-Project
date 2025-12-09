// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "StateTreeTaskBase.h"
#include "STT_FCAttackTarget.generated.h"

/**
 * [Task] 사거리 내에 타겟이 있으면 공격을 시도합니다.
 * 성공 시 Succeeded, 사거리가 안 닿거나 쿨타임이면 Failed를 반환합니다.
 */
USTRUCT(meta = (DisplayName = "FC Attack Target", Category = "FC|Monster|Action"))
struct FABRICATION_API FSTT_FCAttackTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
