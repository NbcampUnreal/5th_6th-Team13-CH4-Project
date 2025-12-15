// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCVanish.generated.h"

/**
 * [BehaviorTree Task] 몬스터를 숨기고 충돌을 끕니다 (Hit & Run 패턴).
 * 공격 후 사용되며, 다음 Task(Wait → Respawn)로 이어집니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCVanish : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCVanish();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};