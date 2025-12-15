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
			ServerRPCSetPlayerNickName(FCGameInstance->GetLocalPlayerNickName());
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

			UFCRoomList_Lobby* NameText = Cast<UFCRoomList_Lobby>(RoomListWidgetInstance);
			if (!IsValid(NameText)) return;

			AFCPlayerState_Lobby* FCPlayerState = GetPlayerState<AFCPlayerState_Lobby>();
			if (!IsValid(FCPlayerState)) return;

			NameText->SetPlayerNickNameText(FCPlayerState->GetPlayerNickName());
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
