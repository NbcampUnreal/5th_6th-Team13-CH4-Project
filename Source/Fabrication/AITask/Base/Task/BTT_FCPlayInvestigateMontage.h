// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITask/Base/Task/BTT_FCPlayMontageBase.h"
#include "BTT_FCPlayInvestigateMontage.generated.h"

/**
 * [BehaviorTree Task] 수색 애니메이션 몽타주를 재생합니다.
 * 플레이어를 놓친 후 마지막 위치에서 주변을 둘러보는 동작입니다.
 * 애니메이션이 끝날 때까지 InProgress 상태를 유지합니다.
 */
UCLASS()
class FABRICATION_API UBTT_FCPlayInvestigateMontage : public UBTT_FCPlayMontageBase
{
	GENERATED_BODY()

public:
	UBTT_FCPlayInvestigateMontage();

protected:
	// UBTT_FCPlayMontageBase 인터페이스 구현
	virtual UAnimMontage* GetMontage(class AFCMonsterBase* Monster) const override;
	virtual void PlayMontage(class AFCMonsterBase* Monster) override;
	virtual void StopMontage(class AFCMonsterBase* Monster) override;
};
