// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FCPlayMontageBase.generated.h"

class UAnimMontage;
class AFCMonsterBase;

/**
 * [BehaviorTree Task] 몽타주 재생 기본 클래스 (Template Method 패턴)
 *
 * 공통 로직:
 * - ExecuteTask: Monster 획득, 몽타주 체크, 델리게이트 바인딩, InProgress 반환
 * - OnMontageEnded: FinishLatentTask 호출
 * - OnTaskFinished: 몽타주 정지, 캐시 리셋
 *
 * 자식 클래스에서 구현해야 할 함수:
 * - GetMontage(): 재생할 몽타주 반환
 * - PlayMontage(): Multicast 애니메이션 재생 함수 호출
 * - StopMontage(): Multicast 애니메이션 정지 함수 호출
 * - GetNullMontageBehavior(): 몽타주가 없을 때 반환할 결과 (선택적)
 */
UCLASS(Abstract)
class FABRICATION_API UBTT_FCPlayMontageBase : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FCPlayMontageBase();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	/**
	 * 재생할 몽타주 반환 (자식 클래스에서 구현 필수)
	 * @param Monster 몬스터 액터
	 * @return 재생할 AnimMontage
	 */
	virtual UAnimMontage* GetMontage(AFCMonsterBase* Monster) const PURE_VIRTUAL(UBTT_FCPlayMontageBase::GetMontage, return nullptr;);

	/**
	 * 몽타주 재생 시작 (자식 클래스에서 구현 필수)
	 * Multicast_PlayXXXAnim() 호출
	 * @param Monster 몬스터 액터
	 */
	virtual void PlayMontage(AFCMonsterBase* Monster) PURE_VIRTUAL(UBTT_FCPlayMontageBase::PlayMontage, );

	/**
	 * 몽타주 정지 (자식 클래스에서 구현 필수)
	 * Multicast_StopXXXAnim() 호출
	 * @param Monster 몬스터 액터
	 */
	virtual void StopMontage(AFCMonsterBase* Monster) PURE_VIRTUAL(UBTT_FCPlayMontageBase::StopMontage, );

	/**
	 * 몽타주가 없을 때 반환할 결과 (선택적 오버라이드)
	 * 기본값: Failed
	 * StunMontage 등 몽타주 없이도 진행해야 하는 경우 Succeeded 반환
	 */
	virtual EBTNodeResult::Type GetNullMontageBehavior() const { return EBTNodeResult::Failed; }

	/** 몽타주 종료 콜백 */
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	/** BehaviorTreeComponent 캐시 (Latent Task 완료용) */
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
