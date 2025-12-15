// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "FCPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UAnimMontage;
class USpeedControlComponent;
class AFlashLight;
class UFC_InventoryComponent;

UCLASS()
class FABRICATION_API AFCPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region AFCPlayerCharacter Override
public:
	AFCPlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region AFCPlayerCharacter Basic
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Flash")
	TSubclassOf<AFlashLight> FlashLigthClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Flash")
	TObjectPtr<AFlashLight> FlashLightInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Inven")
	TObjectPtr<UFC_InventoryComponent> InvenComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;
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

	UFUNCTION()
	void UseItemSlot1(const FInputActionValue& Value);

	UFUNCTION()
	void UseItemSlot2(const FInputActionValue& Value);

	UFUNCTION()
	void UseItemSlot3(const FInputActionValue& Value);

	UFUNCTION()
	void UseItemSlot4(const FInputActionValue& Value);
#pragma endregion

#pragma region Animation

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	TObjectPtr<UAnimMontage> DrinkMontage;

#pragma endregion

#pragma region SpeedControlComponent

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpeedControlComponent> SpeedControlComp;
#pragma endregion

#pragma region SlotFunction
public:

	UFUNCTION(Server, Reliable)
	void Server_AssignQuickSlot(int32 SlotIndex, int32 InvIndex);
	UFUNCTION(Server, Reliable)
	void Server_UseQuickSlot(int32 SlotIndex);
#pragma endregion
#pragma region Function

public:
	UFUNCTION(BlueprintCallable)
	void UpdateSpeedByHP(int32 CurHP);

	UFUNCTION(BlueprintCallable)
	void PlayMontage();

	UFUNCTION()
	void InitalizeFlashLight();

	UFUNCTION()
	void OnPlayerDiedProcessing();

	UFUNCTION()
	void EnableLineTrace();

	UFUNCTION()
	void UseQuickSlotItem(int32 Index);
#pragma endregion

#pragma region Var
protected:
	UPROPERTY(Replicated)
	float CurrentAimPitch;

	float PrevAimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineTrace")
	float LineTraceDist;
#pragma endregion

#pragma region RPC
protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCUpdateAimPitch(float AimPitchValue);

	UFUNCTION(Server, Reliable)
	void ServerRPCPlayMontage();

	UFUNCTION(Client, Reliable)
	void ClientRPCPlayMontage(AFCPlayerCharacter* TargetCharacter);

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeUseFlashLightValue(bool bIsUsing);

#pragma endregion

#pragma region Getter/Setter
public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentAimPith() const { return CurrentAimPitch; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetUsingFlashLight() const { return bUseFlashLight; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetDetectItem() const { return bIsDetectPickUpTrigger; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetDetectItem(bool bIsDetect) { bIsDetectPickUpTrigger = bIsDetect;}

#pragma endregion

#pragma region ReplicatedVar
public:
	UPROPERTY(Replicated)
	bool bUseFlashLight;

	UPROPERTY(Replicated)
	bool bIsDetectPickUpTrigger;
#pragma endregion




};
