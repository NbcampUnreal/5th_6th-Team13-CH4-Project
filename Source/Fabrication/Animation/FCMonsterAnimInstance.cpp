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

	// 숨겨진 상태 (Vanish 상태)
	bIsVanished = OwnerMonster->IsHidden();

	// 공격 중인지 (공격 가능 여부의 반대)
	// 공격 불가능 = 공격 중 or 쿨타임
	bIsAttacking = !OwnerMonster->bCanAttack;
}