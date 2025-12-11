// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeedControlComponent.generated.h"

class AFCPlayerCharacter;
class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FABRICATION_API USpeedControlComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region USpeedControlComponent Override

public:
	USpeedControlComponent();

protected:
	virtual void BeginPlay() override;

#pragma endregion

#pragma region SpeedControlComponent Var/Func

public:
	UFUNCTION(BlueprintCallable)
	void SetSpeedByHealth(int32 CurHP);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> SpeedChangeValueArray;

	UPROPERTY()
	TObjectPtr<AFCPlayerCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMoveComp;
#pragma endregion



};
