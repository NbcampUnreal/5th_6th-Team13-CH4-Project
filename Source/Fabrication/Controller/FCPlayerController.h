// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
class AFCSpectatorPawn;
struct FInputActionValue;
class UFC_InventoryWidget;

UCLASS()
class FABRICATION_API AFCPlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region AFCPlayerController Override

public:
	AFCPlayerController();
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
#pragma endregion

#pragma region PlayerInput
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ItemUseAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Interact;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FirstQuickSlot;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SecondQuickSlot;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ThirdQuickSlot;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FourthQuickSlot;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DropMode;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> NextSpectate;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DropAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> FCInputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> SpectatorMappingContext;

public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFC_InventoryWidget>InventoryWidget;

	UPROPERTY(BlueprintReadOnly)
	UFC_InventoryWidget* InvInstance;
#pragma endregion

#pragma region Ready

public:
	UFUNCTION(BlueprintCallable)
	void ToggleReady();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCSetReady(bool bReady);
	
	virtual void AcknowledgePossession(APawn* P) override;
#pragma endregion

#pragma region Function
public:
	UFUNCTION()
	void OnDieProcessing();

	UFUNCTION()
	void SpectatingSetting();

	UFUNCTION()
	void NextSpectateAction(const FInputActionValue& Value);
#pragma endregion

#pragma region RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCOnDieProcessing();

	UFUNCTION(Server, Reliable)
	void ServerRPCSetNickName(const FString& NickName);

	UFUNCTION(Server,Reliable)
	void ServerRPCNextSpectating();
#pragma endregion

#pragma region DropMode	
public:
	bool bDropMode = false;

	UFUNCTION()
	void SetDropMode(bool IsDropMode);

	UFUNCTION()
	void ToggleDropMode();
#pragma endregion

#pragma region Var
	UPROPERTY()
	int32 SpectateTargetIndex;

	UPROPERTY()
	TObjectPtr<AFCSpectatorPawn> FCSpectatorPawn;
#pragma endregion


};
