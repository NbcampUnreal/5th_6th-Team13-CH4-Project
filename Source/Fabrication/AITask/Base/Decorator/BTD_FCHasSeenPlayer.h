// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_FCHasSeenPlayer.generated.h"

/**
 * [BehaviorTree Decorator] 현재 시야에 플레이어가 보이는지 체크합니다.
 * true: SeenPlayer가 유효함 (현재 눈에 보임)
 * false: SeenPlayer가 없음 (시야에서 사라짐)
 *
 * HasTarget과의 차이:
 * - HasTarget: 추적 대상이 있는지 (시야에서 사라져도 마지막 위치까지 추적)
 * - HasSeenPlayer: 현재 실시간으로 눈에 보이는지
 */
UCLASS()
class FABRICATION_API UBTD_FCHasSeenPlayer : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_FCHasSeenPlayer();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
