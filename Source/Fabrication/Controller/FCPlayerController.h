// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;

UCLASS()
class FABRICATION_API AFCPlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region AFCPlayerController Override

public:
	AFCPlayerController();
	
	virtual void BeginPlay() override;
	
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
	TObjectPtr<UInputMappingContext> FCInputMappingContext;

public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget>InventoryWidget;

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* InvInstance;
#pragma endregion

#pragma region Ready

public:
	UFUNCTION(BlueprintCallable)
	void ToggleReady();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCSetReady(bool bReady);
	
#pragma endregion

#pragma region Function
public:
	UFUNCTION()
	void OnDieProcessing();
#pragma endregion

#pragma region RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCOnDieProcessing();

	UFUNCTION(Client, Reliable)
	void ClientRPCStartSpectating(AActor* PC);
#pragma endregion


};
