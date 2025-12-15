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
#include "Player/FCPlayerCharacter.h"

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
	FCInputMappingContext(nullptr)
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
	}
	if (!InvInstance && InventoryWidget)
	{
		InvInstance = CreateWidget<UUserWidget>(this, InventoryWidget);
		if (InvInstance)
		{
			InvInstance->AddToViewport();
		}
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

void AFCPlayerController::OnDieProcessing()
{
	// 관전 모드 테스트중
	if (IsLocalController())
	{
		ServerRPCOnDieProcessing();
	}
}

void AFCPlayerController::ClientRPCStartSpectating_Implementation(AActor* TargetPawn)
{
	AFCPlayerState* MyPS = GetPlayerState<AFCPlayerState>();
	if (!MyPS) return;

	/*for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AFCPlayerController* TargetController = Cast<AFCPlayerController>(*It))
		{
			AFCPlayerState* TargetPS = TargetController->GetPlayerState<AFCPlayerState>();
			if (TargetPS && TargetController->GetPawn() && TargetController->GetPawn()->IsA(AFCPlayerCharacter::StaticClass()))
			{
				SetViewTargetWithBlend(TargetController, 1.0f);
				return;
			}
		}
	}*/

	if (IsValid(TargetPawn))
	{
		SetViewTargetWithBlend(TargetPawn, 0.1f);
	}

}

void AFCPlayerController::ServerRPCOnDieProcessing_Implementation()
{
	//if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
	//{
	//	if (AFCGameMode* FCGM = Cast<AFCGameMode>(GM))
	//	{
	//		//AFCSpectatorPawn* FCSpecPawn = GetWorld()->SpawnActor<AFCSpectatorPawn>(FCGM->SpectatorClass);	
	//		//Possess(FCSpecPawn);
	//		
	//		const TArray<APlayerController*> AlivePlayerControllerArr = FCGM->AlivePlayerControllers;
	//		APlayerController* TargetPC = nullptr;

	//		for (const auto& PC : AlivePlayerControllerArr)
	//		{
	//			if (PC == this)
	//			{
	//				continue;
	//			}
	//			TargetPC = PC;
	//			break;
	//		}
	//		
	//		SetViewTargetWithBlend(TargetPC, 0.1f);

	//		//ClientRPCStartSpectating(TargetPC->GetViewTarget());

	//		/*if (AFCPlayerState* MyPS = GetPlayerState<AFCPlayerState>())
	//		{
	//			MyPS->bIsDead = true;
	//			FCGM->ChangeSpectatorMode(this);
	//		}*/
	//	}
	//}
}

void AFCPlayerController::ServerRPCSetReady_Implementation(bool bReady)
{
	AFCPlayerState* FCPS = GetPlayerState<AFCPlayerState>();
	if (IsValid(FCPS))
	{
		FCPS->bIsReady = bReady;
	}
}
