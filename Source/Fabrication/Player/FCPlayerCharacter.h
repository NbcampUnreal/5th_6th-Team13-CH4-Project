// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "FCPlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EMontage : uint8
{
	Drinking,
	Die,
	RaiseFlashLight,
	LowerFlashLight
};

UENUM(BlueprintType)
enum class EAttachItem : uint8
{
	None,
	FlashLight,
	Potion
};

UENUM(BlueprintType)
enum class ESoundType : uint8
{
	FootStep,
	TakeDamage,
	PotionDrink,
	FlashLight,
	Die
};

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UAnimMontage;
class USpeedControlComponent;
class AFlashLight;
class UFC_InventoryComponent;
class UStatusComponent;
class AHealingItem;
class ANoiseItem;
class USoundCue;
class UWidgetComponent;
class APickupItemBase;
class AInteratableObjectBase;
class UNiagaraSystem;

UCLASS()
class FABRICATION_API AFCPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region AFCPlayerCharacter Override
public:
	AFCPlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateNickNameWidgetRotation();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
protected: 
	virtual void BeginPlay() override;
#pragma endregion

#pragma region AFCPlayerCharacter Basic
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Flash")
	TSubclassOf<AFlashLight> FlashLigthClass;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Components|Flash")
	TObjectPtr<AFlashLight> FlashLightInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Flash")
	TSubclassOf<AHealingItem> HealItemClass;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Components|Flash")
	TObjectPtr<AHealingItem> HealItemInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Inven")
	TObjectPtr<UFC_InventoryComponent> InvenComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Status")
	TObjectPtr<UStatusComponent> StatusComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UPawnNoiseEmitterComponent> NoiseEmitter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundCue> FootStepSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundAttenuation> FootStepSoundAttenuation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widet")
	TObjectPtr<UWidgetComponent> NickNameWidget;
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

	UFUNCTION()
	void ToggleDropMode(const FInputActionValue& value);

	UFUNCTION()
	void Drop(const FInputActionValue& value);

	UFUNCTION()
	void ToggleFlashLight(const FInputActionValue& value);

	UFUNCTION()
	void ToggleSharedNote(const FInputActionValue& value);

#pragma endregion

#pragma region Animation

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	TArray<TObjectPtr<UAnimMontage>> PlayerMontages;
#pragma endregion

#pragma region SpeedControlComponent

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpeedControlComponent> SpeedControlComp;
#pragma endregion

#pragma region Function

public:
	UFUNCTION(BlueprintCallable)
	void UpdateSpeedByHP(int32 CurHP);

	UFUNCTION(BlueprintCallable)
	void PlayMontage(EMontage MontageType);

	UFUNCTION()
	void InitalizeAttachItem();

	UFUNCTION()
	void OnPlayerDiedProcessing();

	UFUNCTION()
	void EnableLineTrace();

	UFUNCTION()
	void UseQuickSlotItem(int32 SlotIndex);

	UFUNCTION()
	void UsePotionAction();

	UFUNCTION()
	void FootStepAction();
	
	UFUNCTION()
	void OnPlayerDiePreProssessing();
	
	UFUNCTION()
	void UseFlashLight();
	
	UFUNCTION()
	void SetAttachItem(EAttachItem AttachItem, bool bSetHidden);
	
	UFUNCTION()
	void CheckingSelectSlot();
	
	UFUNCTION()
	void OnRep_UsingFlashLight();
	
	UFUNCTION()
	void PlaySound(FVector Location, FRotator Rotation, ESoundType SoundType);
		
	UFUNCTION()
	void OnRep_FlashLightOn();//bFlashLight Copy Client -> 호출 

	UFUNCTION()
	void ChangeUseFlashLightValue(bool bIsUsing);

	UFUNCTION(BlueprintCallable)
	float GetBatteryPercent() const; 

	UFUNCTION(BlueprintCallable)
	float GetCurrentBattery() const; 

	UFUNCTION(BlueprintCallable)
	bool IsFlashLightUseAble() const;
	
	UFUNCTION()
	void RemoveFlashLight(); 

	UFUNCTION()
	void DrawReviveRangeCycle(UWorld* World, const FVector PlayerLocation, float Radius);

	UFUNCTION()
	void UseNoiseItem();
	
	UFUNCTION()
	void HideAllInteractWidgets();

#pragma endregion

#pragma region Var
protected:
	float PrevAimPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineTrace")
	float LineTraceDist;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 CurrentSelectSlotIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TArray<TObjectPtr<USoundCue>> PlayerSounds;

	UPROPERTY(EditDefaultsOnly, Category = "NoiseItem")
	TSubclassOf<ANoiseItem> NoiseItemClass;

	UPROPERTY()
	bool bIsOpenNote = false;

	UPROPERTY(EditDefaultsOnly, Category = "FootStep")
	float FootStepLoudness;
	
	UPROPERTY(EditDefaultsOnly, Category = "FootStep")
	float FootStepMaxRange;

	UPROPERTY(EditDefaultsOnly, Category = "ReviveFX")
	TObjectPtr<UNiagaraSystem> ReviveFX;

	UPROPERTY(EditDefaultsOnly, Category = "Revive | Sound")
	TObjectPtr<USoundBase> ReviveSFX;

	UPROPERTY(EditDefaultsOnly, Category = "Revive | Sound")
	TObjectPtr<USoundAttenuation> ReviveAttenuation;

public:
	UPROPERTY()
	TSet<APickupItemBase*> CurrentOverlappingPickups;
	float BatteryUpdateAcc = 0.0f;
	
	UPROPERTY()
	TSet<AInteratableObjectBase*> CurrentOverlappingIteract;
#pragma endregion

#pragma region RPC
protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCUpdateAimPitch(float AimPitchValue);

	UFUNCTION(Server, Reliable)
	void ServerRPCPlayMontage(EMontage MontageType);

	UFUNCTION(Client, Reliable)
	void ClientRPCPlayMontage(AFCPlayerCharacter* TargetCharacter, EMontage MontageType);

	UFUNCTION(Client, Reliable)
	void ClientRPCFlashLightSetting();

	UFUNCTION(Server, Reliable)
	void Server_UseQuickSlot(int32 Index);

	UFUNCTION(Server, Reliable)
	void Server_AssignQuickSlot(int32 SlotIndex, int32 InvIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCPlayerDieProcessing();

public:
	UFUNCTION(Client, Reliable)
	void ClientRPCSelfPlayMontage(EMontage Montage);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCInteract(AActor* TargetActor, ACharacter* User, const FHitResult& HitResult);
	
	UFUNCTION(Server, UnReliable)
	void ServerRPCPlaySound(FVector Location, FRotator Rotation, ESoundType SoundType);
	
	UFUNCTION(NetMulticast, UnReliable)
	void MulticastRPCPlaySound(FVector Location, FRotator Rotation, ESoundType SoundType, bool bSoundPlaySelf = false);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayMontage(EMontage MontageType);

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeOnFlashLightValue(bool bFlashOn); //FlashLight On/Off State RPC 

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeUseFlashLightValue(bool bIsUsing);

	UFUNCTION(Server, Reliable)
	void ServerRPCToggleFlashLight();

	UFUNCTION(Server,Reliable)
	void ServerToggleEquipFlashlight();//Equip <-> !Equip 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_FlashEquip(); //15% Equip 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_FlashUnEquip(); //85% !Equip 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_FlashTransitionEnd(); //95% End 

	//Revive RPC 
	UFUNCTION(Server, Reliable)
	void ServerRPCPlayerReviveProcessing();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ReviveAnimation();
	
	UFUNCTION(Client, UnReliable)
	void ClientRPCPlaySound(FVector Location, FRotator Rotation, ESoundType SoundType);
	
	UFUNCTION(Client, Reliable)
	void ClientRPCSetIgnoreLookInput();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_PlayReviveFX();

#pragma endregion

#pragma region Getter/Setter
public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentAimPith() const { return CurrentAimPitch; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetUsingFlashLight() const { return bUseFlashLight; }
#pragma endregion

#pragma region ReplicatedVar
public:
	UPROPERTY(ReplicatedUsing = OnRep_UsingFlashLight, BlueprintReadWrite)
	bool bUseFlashLight;
	
	UPROPERTY(Replicated)
	float CurrentAimPitch;

	UPROPERTY(ReplicatedUsing = OnRep_FlashLightOn, BlueprintReadWrite)
	bool bFlashLightOn;//Montage Playing ? 

	UPROPERTY(Replicated,BlueprintReadWrite)
	bool bFlashTransition = false;

	UPROPERTY(Replicated, BlueprintReadWrite) //Changed Montage State  == !bUseFlashLight
	bool bPendingUseFlashLight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DrainRate = 7.0f; 
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bFlashLightUseAble = true; 

	UPROPERTY(Replicated)
	int32 EquippedFlashInvIndex = INDEX_NONE;

#pragma endregion
	UPROPERTY(BlueprintReadWrite)
	float FullDuration;
};
