// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITask/Base/Task/BTT_FCPlayMontageBase.h"
#include "BTT_FCPlayLureInvestigateMontage.generated.h"

/**
 * Lure Investigate용 노드 메모리
 * 시작 시간을 저장하여 MinLureDuration 체크에 사용
 */
USTRUCT()
struct FBTTask_LureInvestigateMemory
{
	GENERATED_BODY()

	/** 태스크 시작 시간 */
	float StartTime = 0.0f;
};

/**
 * [BehaviorTree Task] Lure 도착 후 수색 애니메이션 재생
 *
 * - MinLureDuration 동안 Abort 불가 (어떤 자극에도 반응 안 함)
 * - MinLureDuration 경과 후 Abort 가능 (플레이어 발견, 소리 등에 반응)
 * - 애니메이션 완료 시 Succeeded 반환
 */
UCLASS()
class FABRICATION_API UBTT_FCPlayLureInvestigateMontage : public UBTT_FCPlayMontageBase
{
	GENERATED_BODY()

public:
	UBTT_FCPlayLureInvestigateMontage();

	virtual uint16 GetInstanceMemorySize() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	// UBTT_FCPlayMontageBase 인터페이스 구현
	virtual UAnimMontage* GetMontage(class AFCMonsterBase* Monster) const override;
	virtual void PlayMontage(class AFCMonsterBase* Monster) override;
	virtual void StopMontage(class AFCMonsterBase* Monster) override;

private:
	/** 경과 시간 계산 헬퍼 */
	float GetElapsedTime(const UBehaviorTreeComponent& OwnerComp, const uint8* NodeMemory) const;
};
