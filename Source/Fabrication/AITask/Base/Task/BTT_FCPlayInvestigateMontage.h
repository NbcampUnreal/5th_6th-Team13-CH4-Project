// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCPlayInvestigateMontage.generated.h"

/**
 * [BehaviorTree Task] 수색 애니메이션 몽타주를 재생합니다.
 * 플레이어를 놓친 후 마지막 위치에서 주변을 둘러보는 동작입니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCPlayInvestigateMontage : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCPlayInvestigateMontage();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
