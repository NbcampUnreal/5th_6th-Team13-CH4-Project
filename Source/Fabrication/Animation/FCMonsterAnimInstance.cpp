// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/FCMonsterAnimInstance.h"
#include "Monster/FCMonsterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFCMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 소유자 몬스터 캐스팅
	OwnerMonster = Cast<AFCMonsterBase>(GetOwningActor());

	if (IsValid(OwnerMonster))
	{
		// 이동 컴포넌트 레퍼런스 저장
		OwnerMovementComp = OwnerMonster->GetCharacterMovement();
	}
}

void UFCMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 유효성 검사
	if (!IsValid(OwnerMonster) || !IsValid(OwnerMovementComp))
	{
		return;
	}

	// === 이동 관련 변수 업데이트 ===

	// 속도 벡터
	Velocity = OwnerMovementComp->Velocity;

	// 이동 속도 (수평 속도만 계산)
	MoveSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();

	// === AI 상태 변수 업데이트 ===

	// 추격 중인지 (타겟이 있으면 추격 중)
	bIsChasing = (OwnerMonster->TargetPlayer != nullptr);

	// 스턴 상태
	bIsStunned = OwnerMonster->bIsStunned;

	// Vanish 상태 (공격 후 사라진 상태)
	bIsVanished = OwnerMonster->bIsVanished;

	// 공격 중인지 (공격 불가 + Vanish 상태가 아닐 때)
	// bCanAttack=false는 공격 중 또는 Vanish 상태를 의미
	// Vanish 상태에서는 공격 애니메이션이 필요 없으므로 제외
	bIsAttacking = !OwnerMonster->bCanAttack && !OwnerMonster->bIsVanished;
}