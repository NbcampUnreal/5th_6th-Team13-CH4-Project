// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FCMonsterAnimInstance.generated.h"

class AFCMonsterBase;
class UCharacterMovementComponent;

/**
 * 몬스터 전용 애니메이션 인스턴스
 *
 * AI 상태(추격, 공격, 스턴)에 따라 애니메이션을 제어합니다.
 * BehaviorTree의 상태를 읽어서 애니메이션 블루프린트에 전달합니다.
 *
 * [사용법]
 * 1. Animation Blueprint 생성 (ABP_Monster)
 * 2. Parent Class를 UFCMonsterAnimInstance로 설정
 * 3. State Machine에서 아래 변수들을 사용:
 *    - MoveSpeed: Idle/Walk/Chase 블렌드
 *    - bIsChasing: 추격 애니메이션
 *    - bIsAttacking: 공격 애니메이션
 *    - bIsStunned: 스턴 애니메이션
 */
UCLASS()
class FABRICATION_API UFCMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

#pragma region UAnimInstance Override

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

#pragma endregion

#pragma region Monster References

protected:
	/** 소유자 몬스터 캐릭터 */
	UPROPERTY()
	TObjectPtr<AFCMonsterBase> OwnerMonster;

	/** 소유자의 이동 컴포넌트 */
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerMovementComp;

#pragma endregion

#pragma region Animation Variables (Blueprint Readable)

protected:
	/** 현재 이동 속도 (cm/s) - Idle/Walk/Chase 블렌드에 사용 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
	float MoveSpeed;

	/** 현재 속도 벡터 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
	FVector Velocity;

	/** 추격 중인지 여부 (TargetPlayer가 있으면 true) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|State")
	bool bIsChasing;

	/** 공격 중인지 여부 (공격 몽타주 재생용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|State")
	bool bIsAttacking;

	/** 스턴 상태인지 여부 (스턴 애니메이션 재생용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|State")
	bool bIsStunned;

	/** 숨겨진 상태인지 여부 (Vanish/Respawn 사이클) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|State")
	bool bIsVanished;

#pragma endregion
};