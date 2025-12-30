// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITask/Base/Task/BTT_FCPlayMontageBase.h"
#include "BTT_FCPlayStunMontage.generated.h"

/**
 * [BehaviorTree Task] 스턴 애니메이션 몽타주를 재생합니다.
 * 스턴 상태일 때 재생되는 애니메이션입니다.
 * 애니메이션이 끝날 때까지 InProgress 상태를 유지합니다.
 *
 * 특이사항: 몽타주가 없어도 Succeeded 반환 (스턴은 적용되지만 애니메이션만 없음)
 */
UCLASS()
class FABRICATION_API UBTT_FCPlayStunMontage : public UBTT_FCPlayMontageBase
{
	GENERATED_BODY()

public:
	UBTT_FCPlayStunMontage();

protected:
	// UBTT_FCPlayMontageBase 인터페이스 구현
	virtual UAnimMontage* GetMontage(class AFCMonsterBase* Monster) const override;
	virtual void PlayMontage(class AFCMonsterBase* Monster) override;
	virtual void StopMontage(class AFCMonsterBase* Monster) override;

	// 몽타주가 없어도 성공 반환 (스턴은 적용됨)
	virtual EBTNodeResult::Type GetNullMontageBehavior() const override { return EBTNodeResult::Succeeded; }
};
