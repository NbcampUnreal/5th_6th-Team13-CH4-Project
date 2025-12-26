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
#include "Player/Components/UI/FC_PlayerHealth.h"
#include "Flash/UI/FC_FlashLightBattery.h"
#include "Net/UnrealNetwork.h"
#include "GameState/UI/FC_NoteWidget.h"

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
	DropAction(nullptr),
	OnFlashLight(nullptr),
	FCInputMappingContext(nullptr),
	SpectatorMappingContext(nullptr),
	SpectateTargetIndex(0)
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
	if (!HealthWidgetInstance && HealthWidget)
	{
		HealthWidgetInstance = CreateWidget<UFC_PlayerHealth>(this, HealthWidget);
		if (HealthWidgetInstance)
		{
			HealthWidgetInstance->AddToViewport();
		}
	}
	if (!BatteryWidgetInstance && BatteryWidgetClass)
	{
		BatteryWidgetInstance = CreateWidget<UFC_FlashLightBattery>(this, BatteryWidgetClass);
		if (BatteryWidgetInstance)
		{
			BatteryWidgetInstance->AddToViewport();
			BatteryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (!NoteWidgetInstance && NoteWidget)
	{
		NoteWidgetInstance = CreateWidget<UFC_NoteWidget>(this, NoteWidget);
		if (NoteWidgetInstance)
		{
			NoteWidgetInstance->AddToViewport();
			NoteWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
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

void AFCPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	if (AFCPlayerCharacter* FCPlayerCharacter = Cast<AFCPlayerCharacter>(aPawn))
	{
		PossessCharacter = FCPlayerCharacter;
	}
}

void AFCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, PossessCharacter);
}

void AFCPlayerController::SetNoteMode(bool IsNote)
{
	if (!NoteWidgetInstance) return;

	if (IsNote)
	{
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);

		InputMode.SetWidgetToFocus(NoteWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		bShowMouseCursor = true; 
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);
	}
	else
	{
		FInputModeGameOnly Inputmode;
		SetInputMode(Inputmode);

		bShowMouseCursor = false; 
		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
	}
}

void AFCPlayerController::CloseNote()
{
	if (!IsLocalController() || !NoteWidgetInstance) return;

	NoteWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	SetNoteMode(false);
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
	if (IsLocalController())
	{
		SpectatingSetting();
		ServerRPCOnDieProcessing();
		SetIgnoreLookInput(false);
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

	if (IsValid(InvInstance))
	{
		InvInstance->RemoveFromViewport();
	}
}

void AFCPlayerController::ExitSpectatorSetting()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnSubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EnSubSystem->ClearAllMappings(); 
			EnSubSystem->AddMappingContext(FCInputMappingContext, 0);//기존 입력 매핑 추가  
		}
	}

	if (!IsValid(InvInstance))
	{
		if (InventoryWidget) {
			InvInstance = CreateWidget<UFC_InventoryWidget>(this, InventoryWidget);
			if (InvInstance) {
				InvInstance->AddToViewport();
			}
		}
	}
	else
	{
		InvInstance->AddToViewport();
	}

	//카메라를 플레이어로 재 설정 
	if (AFCPlayerCharacter* PR = Cast<AFCPlayerCharacter>(GetPawn()))
	{
		SetViewTarget(PR);
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

void AFCPlayerController::CreateBatteryWidget()
{
	if (!IsLocalController()) return;

	if (BatteryWidgetInstance)
	{
		BatteryWidgetInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void AFCPlayerController::RemoveBatteryWidget()
{
	if (!IsLocalController()) return;

	if (BatteryWidgetInstance)
	{
		BatteryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AFCPlayerController::ShowItemDescription(const FName ID)
{
	if (!DescriptionInstance || bIsFadingOut) return;

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
			DescriptionInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
		}

		//Play FadeInAnim  
		FName FucName = FName("PlayFadeIn");
		if (DescriptionInstance->GetClass()->IsFunctionImplementedInScript(FucName))
		{
			DescriptionInstance->ProcessEvent(DescriptionInstance->FindFunction(FucName), nullptr);
		}
		bDescVisible = true;
		LastDescItemID = ID;

		GetWorldTimerManager().ClearTimer(DescHideHandle);
		GetWorldTimerManager().SetTimer(
			DescHideHandle, [this]() {HideItemDescription();
			}, 2.0f, false);
	}
}

void AFCPlayerController::HideItemDescription()
{
	if (!DescriptionInstance || !bDescVisible || bIsFadingOut) return;

	bIsFadingOut = true; 

	//Play FadeOutAnim
	FName FucName = FName("PlayFadeOut");
	if (DescriptionInstance->GetClass()->IsFunctionImplementedInScript(FucName))
	{
		DescriptionInstance->ProcessEvent(DescriptionInstance->FindFunction(FucName),nullptr);
	}
	bDescVisible = false;
	LastDescItemID = NAME_None;
	
	GetWorldTimerManager().ClearTimer(FadeResetHandle);
	GetWorldTimerManager().SetTimer(FadeResetHandle, [this]() {
		bIsFadingOut = false;
		}, 0.4f, false);
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
			FCGM->PlayerDead(this);
			const TArray<APlayerController*> AlivePlayerControllerArr = FCGM->GetPlayerControllerArray();
			APlayerController* TargetPC = nullptr;
			

			// 플레이어가 1명이거나 살아있는 사람이 1명이라면 관전 시점 대상 없으므로 return
			if (AlivePlayerControllerArr.Num() <= 0)
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

			// if (TargetPC && TargetPC->GetPawn())
			// {
			// 	SetViewTargetWithBlend(TargetPC->GetPawn(), 0.2f);
			// }
			
			
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
		/*FCSpectatorPawn->SetSpectateTarget(PC->GetPawn());*/
		if (APawn* TargetPawn = PC->GetPawn())
		{
			SetViewTargetWithBlend(TargetPawn, 0.2f);
		}
		return;
	}
}

void AFCPlayerController::ReviveAction()
{

	if (!PossessCharacter) return;

	if (FCSpectatorPawn)
	{
		FCSpectatorPawn->Destroy();
		FCSpectatorPawn = nullptr;
	}

	UnPossess();
	Possess(PossessCharacter);

	ClientRPCReviveSetting(PossessCharacter);
}

//void AFCPlayerController::ClientRPCShowNote_Implementation(int32 noteid)
//{
//	UE_LOG(LogTemp, Error, TEXT("[Client] 받은 쪽지 NoteID: %d"), noteid);
//
//	if (!IsLocalController()) return;
//
//	if (!NoteWidgetInstance) return;
//
//	if (!NoteDataTable) return;
//
//	//RowName 생성
//	FName RowName = FName(*FString::Printf(TEXT("Note_%d"), noteid));
//
//	// DataTable에서 찾기
//	FNoteData* NoteData = NoteDataTable->FindRow<FNoteData>(RowName, "");
//	if (!NoteData) return;
//
//	UFC_NoteWidget* NoteWG = Cast<UFC_NoteWidget>(NoteWidgetInstance);
//	if (!NoteWG) return;
//
//	// UI 업데이트
//	if (NoteWG->Note_Text)
//	{
//		NoteWG->Note_Text->SetText(NoteData->Context);
//	}
//
//	NoteWidgetInstance->SetVisibility(ESlateVisibility::Visible);
//	SetNoteMode(true);
//	
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
//			FString::Printf(TEXT("쪽지 UI 표시 완료: %s"), NoteData->bIsTruth ? TEXT("진실") : TEXT("거짓")));
//	}
//}

void AFCPlayerController::ClientRPCReviveSetting_Implementation(AFCPlayerCharacter* PossessPlayerCharacter)
{
	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* EnSubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				EnSubSystem->ClearAllMappings();
				EnSubSystem->AddMappingContext(FCInputMappingContext, 0);
			}
		}
	
		if (IsValid(InvInstance))
		{
			InvInstance->AddToViewport();
		}
	}
	
	// if (AFCPlayerCharacter* PlayerCharacter = Cast<AFCPlayerCharacter>(PossessCharacter))
	// {
	// 	PossessCharacter->PlayerReviveProcessing();
	// }
}
