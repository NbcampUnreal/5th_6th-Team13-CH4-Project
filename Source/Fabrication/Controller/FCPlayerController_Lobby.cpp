#include "Controller/FCPlayerController_Lobby.h"
#include "GameInstance/FCGameInstance.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "GameMode/FCGameMode_Lobby.h"
#include "UI/FCChatting_Lobby.h"
#include "UI/FCRoomList_Lobby.h"
#include "UI/FCRoomWaiting_Lobby.h"

void AFCPlayerController_Lobby::ServerRPCSetPlayerNickName_Implementation(const FString& InNickName)
{
	AFCPlayerState_Lobby* FCPlayerState = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(FCPlayerState))
	{
		FCPlayerState->SetPlayerNickName(InNickName);
	}

	if (AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>())
	{
		GM->MulticastLogNickName(FCPlayerState->GetPlayerNickName());
	}
}

void AFCPlayerController_Lobby::ServerRPCSendChatMessage_Implementation(const FString& Message)
{
}

void AFCPlayerController_Lobby::ClientRPCAddChatMessage_Implementation(const FString& Message)
{
}

void AFCPlayerController_Lobby::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	if (IsLocalPlayerController())
	{
		UFCGameInstance* FCGameInstance = GetGameInstance<UFCGameInstance>();
		if (IsValid(FCGameInstance))
		{
			ServerRPCSetPlayerNickName(FCGameInstance->GetPlayerNickName());
		}

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		bShowMouseCursor = true;
	}
	
	if (IsValid(ChatWidgetClass))
	{
		ChatWidgetInstance = CreateWidget<UUserWidget>(this, ChatWidgetClass);
		
		if (IsValid(ChatWidgetInstance))
		{
			ChatWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(RoomListWidgetClass))
	{
		RoomListWidgetInstance = CreateWidget<UUserWidget>(this, RoomListWidgetClass);

		if (IsValid(RoomListWidgetInstance))
		{
			RoomListWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(RoomWaitingWidgetClass))
	{
		RoomWaitingWidgetInstance = CreateWidget<UUserWidget>(this, RoomWaitingWidgetClass);
		if (IsValid(RoomWaitingWidgetInstance))
		{
			RoomWaitingWidgetInstance->AddToViewport();
		}
	}

}
