// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FCAnimInstance.generated.h"

class AFCPlayerCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API UFCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

#pragma region UFCAnimInstance Override Function

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
#pragma endregion

#pragma region UFCAnimInstance Basic Var/Func

protected:
	UPROPERTY()
	TObjectPtr<AFCPlayerCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Direction;
#pragma endregion



	
};
