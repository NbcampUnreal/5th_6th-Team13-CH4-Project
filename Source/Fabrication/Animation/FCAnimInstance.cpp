// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FCAnimInstance.h"
#include "Player/FCPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<AFCPlayerCharacter>(GetOwningActor());

	if (IsValid(OwnerCharacter))
	{
		OwnerCharacterMovementComp = OwnerCharacter->GetCharacterMovement();
	}

}

void UFCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwnerCharacterMovementComp))
	{
		Velocity = OwnerCharacterMovementComp->Velocity;
		GroundSpeed = FVector(Velocity.X, Velocity.Y, 0).Size();
		bShouldMove = ((OwnerCharacterMovementComp->GetCurrentAcceleration().IsNearlyZero()) == false && (3.f < GroundSpeed));
	}
}
