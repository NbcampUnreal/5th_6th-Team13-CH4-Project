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
class UFC_DescriptionWidget;
class UFC_PlayerHealth;
class UFC_FlashLightBattery;
class AFCPlayerCharacter;
class UFC_NoteWidget;

UCLASS()
class FABRICATION_API AFCPlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region AFCPlayerController Override

public:
	AFCPlayerController();
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
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

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> OnFlashLight;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> FCInputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> SpectatorMappingContext;

public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFC_InventoryWidget>InventoryWidget;

	UPROPERTY(BlueprintReadOnly)
	UFC_InventoryWidget* InvInstance;

	UPROPERTY(EditAnywhere, Category="Description")
	TSubclassOf<UFC_DescriptionWidget> DescriptionWidget;

	UPROPERTY(BlueprintReadOnly)
	UFC_DescriptionWidget* DescriptionInstance;

	UPROPERTY(EditAnywhere, Category="HealthUI")
	TSubclassOf<UFC_PlayerHealth> HealthWidget; 

	UPROPERTY(BlueprintReadWrite)
	UFC_PlayerHealth* HealthWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Flash|Battery")
	TSubclassOf<UFC_FlashLightBattery> BatteryWidgetClass;

	UPROPERTY()
	TObjectPtr<UFC_FlashLightBattery> BatteryWidgetInstance;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Note")
	TSubclassOf<UFC_NoteWidget> NoteWidget;
	
	UPROPERTY()
	TObjectPtr<UFC_NoteWidget> NoteWidgetInstance;
	// 쪽지 데이터를 가져올 DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Note")
	UDataTable* NoteDataTable;

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

	UFUNCTION()
	void ExitSpectatorSetting();

	UFUNCTION()
	void CreateBatteryWidget();

	UFUNCTION()
	void RemoveBatteryWidget();

	UFUNCTION()
	void SetNoteMode(bool IsNote);

	UFUNCTION()
	void CloseNote();
#pragma endregion

#pragma region Hover & KeyBoard Description Function
	UFUNCTION(BlueprintCallable)
	void ShowItemDescription(const FName ID);

	UFUNCTION(BlueprintCallable)
	void HideItemDescription();

	//Hover & KeyBoard Description Fuc 
	UFUNCTION(BlueprintCallable)
	void RequestShowDescription(FName ID);

	UFUNCTION(BlueprintCallable)
	void RequestHideDescription(float Delay); 

	UFUNCTION(BlueprintCallable)
	void UnHoverDescription(FName ID, float Delay = 0.3); 

	//KeyBoard Description Fuc 
	UFUNCTION()
	void RemoveDescription();
#pragma endregion

#pragma region RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCOnDieProcessing();

	UFUNCTION(Server, Reliable)
	void ServerRPCSetNickName(const FString& NickName);

	UFUNCTION(Server,Reliable)
	void ServerRPCNextSpectating();
	
	UFUNCTION(BlueprintCallable)
	void ReviveAction();
	
	UFUNCTION(Client, Reliable)
	void ClientRPCReviveSetting(AFCPlayerCharacter* PossessPlayerCharacter);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowNote(int32 ID);

#pragma endregion

#pragma region DropMode	
public:
	UPROPERTY(BlueprintReadWrite)
	bool bDropMode = false;

	UFUNCTION()
	void SetDropMode(bool IsDropMode);

	UFUNCTION()
	void ToggleDropMode();
#pragma endregion

#pragma region Description
	UPROPERTY()
	bool bDescVisible = false;

	UPROPERTY()
	bool bIsFadingOut = false;

	FName LastDescItemID = NAME_None;
	FName HoveredItemID = NAME_None;

	FTimerHandle DescHideHandle;
	FTimerHandle FadeResetHandle;
#pragma endregion

#pragma region Var
	UPROPERTY()
	int32 SpectateTargetIndex;

	UPROPERTY()
	TObjectPtr<AFCSpectatorPawn> FCSpectatorPawn;
	
	UPROPERTY(Replicated)
	TObjectPtr<AFCPlayerCharacter> PossessCharacter;
#pragma endregion
};
