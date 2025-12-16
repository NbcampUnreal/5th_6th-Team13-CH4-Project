// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCFindClosestPlayer.generated.h"

/**
 * [BehaviorTree Task] 가장 가까운 플레이어를 찾아서 TargetPlayer로 설정합니다.
 * BlinkHunter 전용 - Perception 대신 직접 거리 계산으로 타겟 설정
 */
UCLASS()
class FABRICATION_API UBTT_FCFindClosestPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCFindClosestPlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
