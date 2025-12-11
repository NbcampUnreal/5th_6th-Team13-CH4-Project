// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FCPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UAnimMontage;
class UFC_InventoryComponent;

UCLASS()
class FABRICATION_API AFCPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region AFCPlayerCharacter Override
public:
	AFCPlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region AFCPlayerCharacter Basic
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UFC_InventoryComponent> InventoryComponent;
	
#pragma endregion

#pragma region InputFunc
protected:
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void ItemUse(const FInputActionValue& Value);

	UFUNCTION()
	void Interaction(const FInputActionValue& Value);
#pragma endregion

#pragma region Animation

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	TObjectPtr<UAnimMontage> DrinkMontage;

#pragma endregion

#pragma region SpeedControlComponent



#pragma endregion

};
