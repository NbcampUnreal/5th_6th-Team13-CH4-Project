// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCRespawn.generated.h"

/**
 * [BehaviorTree Task] 몬스터를 안전한 위치에 재등장시킵니다.
 * Vanish → Wait 후 실행되며, 플레이어로부터 일정 거리 떨어진 곳에 스폰합니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCRespawn : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCRespawn();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** 플레이어와 일정 거리 이상 떨어진 안전한 네비게이션 위치 검색 */
	bool GetRandomSpawnLocation(class AFCMonsterBase* Monster, FVector& OutLocation);
};