// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/FCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerState/FCPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Controller/FCPlayerCameraManager.h"
#include "Controller/FCSpectatorPawn.h"
#include "GameMode/FCGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "GameInstance/FCGameInstance.h"
#include "Player/FCPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "Items/Inventory/UI/FC_InventoryWidget.h"
#include "Items/Data/ItemData.h"
#include "Items/Inventory/UI/FC_DescriptionWidget.h"
#include "Items/Inventory/FC_InventoryComponent.h"


AFCPlayerController::AFCPlayerController() :
	MoveAction(nullptr),
	LookAction(nullptr),
	ItemUseAction(nullptr),
	Interact(nullptr),
	FirstQuickSlot(nullptr),
	SecondQuickSlot(nullptr),
	ThirdQuickSlot(nullptr),
	FourthQuickSlot(nullptr),
	DropMode(nullptr),
	NextSpectate(nullptr),
	FCInputMappingContext(nullptr),
	SpectatorMappingContext(nullptr),
	SpectateTargetIndex(0),
	DropAction(nullptr)
{
	// 플레이어 Pitch 조정을 위해 사용(-70~70)
	PlayerCameraManagerClass = AFCPlayerCameraManager::StaticClass();
}

void AFCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{
		return;
	}
	
	FInputModeGameOnly GameOnly;
	SetInputMode(GameOnly);
	
	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* EnSubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				EnSubSystem->AddMappingContext(FCInputMappingContext, 0);
			}
		}
		
		UFCGameInstance* FCGI = GetGameInstance<UFCGameInstance>();
		if (IsValid(FCGI))
		{
			ServerRPCSetNickName(FCGI->GetLocalPlayerNickName());
		}
	}
	if (!InvInstance && InventoryWidget)
	{
		InvInstance = CreateWidget<UFC_InventoryWidget>(this, InventoryWidget);
		if (InvInstance)
		{
			InvInstance->AddToViewport();
		}
	}
	if (!DescriptionInstance && DescriptionWidget)
	{
		DescriptionInstance = CreateWidget<UFC_DescriptionWidget>(this, DescriptionWidget);
		if (DescriptionInstance)
		{
			DescriptionInstance->AddToViewport();
			DescriptionInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AFCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnInputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnInputComp->BindAction(NextSpectate, ETriggerEvent::Started, this, &AFCPlayerController::NextSpectateAction);
	}
}

void AFCPlayerController::ToggleReady()
{
	AFCPlayerState* FCPS = GetPlayerState<AFCPlayerState>();
	if (IsValid(FCPS))
	{
		bool bNewReady = !FCPS->bIsReady;
		ServerRPCSetReady(bNewReady);
	}
}

void AFCPlayerController::ServerRPCSetReady_Implementation(bool bReady)
{
	AFCPlayerState* FCPS = GetPlayerState<AFCPlayerState>();
	if (IsValid(FCPS))
	{
		FCPS->bIsReady = bReady;
	}
}

void AFCPlayerController::SetDropMode(bool IsDropMode)
{
	if (!InvInstance) return;

	if (IsDropMode)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(InvInstance->TakeWidget());
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		bShowMouseCursor = true; 
		SetIgnoreLookInput(true);
		SetIgnoreMoveInput(true);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		
		bShowMouseCursor = false; 
		SetIgnoreLookInput(false);
		SetIgnoreMoveInput(false);
	}
}

void AFCPlayerController::ToggleDropMode()
{
	bDropMode = !bDropMode;
	SetDropMode(bDropMode);
}

void AFCPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	
	if (IsLocalController())
	{
		ServerRPCSetReady(true);
		UE_LOG(LogTemp, Warning, TEXT("게임 맵 로딩 완료: bIsReady = true"));
	}
}

void AFCPlayerController::OnDieProcessing()
{
	// 관전 모드 테스트중
	if (IsLocalController())
	{
		SpectatingSetting();
		ServerRPCOnDieProcessing();
	}
}

void AFCPlayerController::SpectatingSetting()
{
	// 플레이어가 1명이거나 살아있는 사람이 1명이라도 관전자 모드로 변경되어야하기 때문에 이 부분은 예외 처리 하지 않음 
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnSubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EnSubSystem->ClearAllMappings();
			EnSubSystem->AddMappingContext(SpectatorMappingContext, 0);
		}
	}
	if (AFCPlayerState* FCPS = GetPlayerState<AFCPlayerState>())
	{
		FCPS->bIsDead = true;
	}
	
	if (IsValid(InvInstance))
	{
		InvInstance->RemoveFromViewport();
	}
}

void AFCPlayerController::NextSpectateAction(const FInputActionValue& Value)
{
	if (AFCPlayerState* FCPS = GetPlayerState<AFCPlayerState>())
	{
		if (FCPS->bIsDead)
		{
			ServerRPCNextSpectating();
		}
	}
}

void AFCPlayerController::ShowItemDescription(const FName ID)
{
	if (!DescriptionInstance) return;

	//ItemID 중복 호출 방지 
	if (bDescVisible && LastDescItemID == ID) return; 
	
	AFCPlayerCharacter* PR = Cast<AFCPlayerCharacter>(GetPawn());
	if (!PR) return;

	UFC_InventoryComponent* InvenComp = PR->FindComponentByClass<UFC_InventoryComponent>();
	if (!InvenComp) return;

	UDataTable* ItemDataTable = InvenComp->ItemDataTable;
	if (!ItemDataTable) return;

	const FItemData* RowName = ItemDataTable->FindRow<FItemData>(ID, "");
	if (!RowName) return;
	
	if (UFC_DescriptionWidget* DescWidget = Cast<UFC_DescriptionWidget>(DescriptionInstance))
	{
		DescWidget->SetDescriptionText(RowName->Description);
		if (!bDescVisible)
		{
			DescWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			bDescVisible = true;
		}
		LastDescItemID = ID;
	}
}

void AFCPlayerController::HideItemDescription()
{
	if (DescriptionInstance)
	{
		if (bDescVisible)
		{
			DescriptionInstance->SetVisibility(ESlateVisibility::Collapsed);
			bDescVisible = false;
			LastDescItemID = NAME_None;
		}
	}
}

void AFCPlayerController::RequestShowDescription(FName ID)
{
	HoveredItemID = ID; 

	GetWorldTimerManager().ClearTimer(DescHideHandle);

	ShowItemDescription(ID);
}

void AFCPlayerController::RequestHideDescription(float Delay)
{
	GetWorldTimerManager().ClearTimer(DescHideHandle);

	GetWorldTimerManager().SetTimer(
		DescHideHandle,
		[this]() {
			if (HoveredItemID == NAME_None)
			{
				HideItemDescription();
			}
		}, 
		Delay,false
	);
}

void AFCPlayerController::UnHoverDescription(FName ID, float Delay)
{
	if (HoveredItemID == ID)
	{
		HoveredItemID = NAME_None;
	}
	RequestHideDescription(Delay);
}

void AFCPlayerController::RemoveDescription()
{
	HoveredItemID = NAME_None;
	RequestHideDescription(0.2f);
}

void AFCPlayerController::ServerRPCSetNickName_Implementation(const FString& NickName)
{
	AFCPlayerState* FCPS = GetPlayerState<AFCPlayerState>();
	
	if (IsValid(FCPS))
	{
		FCPS->SetPlayerNickName(NickName);
	}
}

void AFCPlayerController::ServerRPCOnDieProcessing_Implementation()
{
	if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
	{
		if (AFCGameMode* FCGM = Cast<AFCGameMode>(GM))
		{
			const TArray<APlayerController*> AlivePlayerControllerArr = FCGM->GetPlayerControllerArray();
			APlayerController* TargetPC = nullptr;
			

			// 플레이어가 1명이거나 살아있는 사람이 1명이라면 관전 시점 대상 없으므로 return
			if (AlivePlayerControllerArr.Num() <= 1)
			{
				return;
			}

			for (int32 i = 0; i < AlivePlayerControllerArr.Num(); ++i)
			{
				if (AlivePlayerControllerArr[i] == this)
				{
					continue;
				}
				TargetPC = AlivePlayerControllerArr[i];
				SpectateTargetIndex = i;
				break;
			}
			
			FCSpectatorPawn = GetWorld()->SpawnActor<AFCSpectatorPawn>(FCGM->SpectatorClass);

			UnPossess();
			Possess(FCSpectatorPawn);
			FCSpectatorPawn->SetSpectateTarget(TargetPC->GetPawn());
		}
	}
}

void AFCPlayerController::ServerRPCNextSpectating_Implementation()
{
	AFCGameMode* FCGM = GetWorld()->GetAuthGameMode<AFCGameMode>();
	if (!FCGM) return;

	const TArray<APlayerController*>& Players = FCGM->GetPlayerControllerArray();
	const int32 PlayerCount = Players.Num();

	if (PlayerCount <= 1) return;

	for (int32 Offset = 1; Offset <= PlayerCount; ++Offset)
	{
		int32 NewIndex = (SpectateTargetIndex + Offset) % PlayerCount;

		APlayerController* PC = Players[NewIndex];
		if (!IsValid(PC)) continue;
		if (PC == this) continue;

		AFCPlayerState* PS = PC->GetPlayerState<AFCPlayerState>();
		if (!PS || PS->bIsDead) continue;

		SpectateTargetIndex = NewIndex;
		FCSpectatorPawn->SetSpectateTarget(PC->GetPawn());
		return;
	}
}
