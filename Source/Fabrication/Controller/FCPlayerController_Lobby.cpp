#include "Controller/FCPlayerController_Lobby.h"
#include "GameInstance/FCGameInstance.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "GameMode/FCGameMode_Lobby.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UI/FCHUD_Lobby.h"
#include "UI/FCChatting_Lobby.h"
#include "UI/FCRoomList_Lobby.h"
#include "UI/FCRoomWaiting_Lobby.h"

AFCPlayerController_Lobby::AFCPlayerController_Lobby()
	: HUD_LobbyClass(nullptr)
	, HUD_Lobby(nullptr)
{
}

void AFCPlayerController_Lobby::ServerRPCSetPlayerNickName_Implementation(const FString& InNickName)
{
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(PS))
	{
		PS->SetPlayerNickName(InNickName);
	}
}

void AFCPlayerController_Lobby::ServerRPCSendChatMessage_Implementation(const FString& Message)
{
	AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>();
	if (IsValid(GM))
	{
		GM->SendChatMessage(Message);
	}
}

void AFCPlayerController_Lobby::ClientRPCAddChatMessage_Implementation(const FString& Message)
{
	AddChatMessage(Message);
}

void AFCPlayerController_Lobby::UpdateNickNameUI(const FString& InNickName)
{
	if (IsValid(HUD_Lobby))
	{
		UFCRoomList_Lobby* RoomList = HUD_Lobby->GetWidget<UFCRoomList_Lobby>();
		if (!IsValid(RoomList)) return;

		FText DisplayNickName = FText::FromString(InNickName);
		RoomList->SetPlayerNickNameText(DisplayNickName);
	}
}

void AFCPlayerController_Lobby::SetChatMessage(const FString& Message)
{
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (!IsValid(PS)) return;

	FString PlayerMessage = FString::Printf(TEXT("%s : %s"), *PS->GetPlayerNickName(), *Message);

	ServerRPCSendChatMessage(PlayerMessage);
}

void AFCPlayerController_Lobby::AddChatMessage(const FString& Message)
{
	if (IsValid(HUD_Lobby))
	{
		UFCChatting_Lobby* Chatting = HUD_Lobby->GetWidget<UFCChatting_Lobby>();
		if (!IsValid(Chatting)) return;

		FText DisplayMessage = FText::FromString(Message);
		Chatting->AddChatMessage(DisplayMessage);
	}

}

void AFCPlayerController_Lobby::OnNickNameUpdated()
{
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (!IsValid(PS)) return;

	UpdateNickNameUI(PS->GetPlayerNickName());
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
		UFCGameInstance* GI = GetGameInstance<UFCGameInstance>();
		if (IsValid(GI))
		{
			ServerRPCSetPlayerNickName(GI->GetLocalPlayerNickName());
		}
	}
	
	if (IsValid(HUD_LobbyClass))
	{
		HUD_Lobby = CreateWidget<UFCHUD_Lobby>(this, HUD_LobbyClass);
		if (IsValid(HUD_Lobby))
		{
			HUD_Lobby->AddToViewport();

			UFCChatting_Lobby* Chatting = HUD_Lobby->GetWidget<UFCChatting_Lobby>();
			if (!IsValid(Chatting)) return;
			
			Chatting->OnChatCommitted.BindUObject(
				this,
				&AFCPlayerController_Lobby::SetChatMessage
			);

			FInputModeUIOnly InputMode;

			InputMode.SetWidgetToFocus(HUD_Lobby->GetCachedWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);

			bShowMouseCursor = true;
		}
	}

}

#pragma region Ready

void AFCPlayerController_Lobby::ToggleReady()
{
	AFCPlayerState_Lobby* FCPS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(FCPS))
	{
		bool bNewReady = !FCPS->bIsReady;
		ServerRPCSetReady(bNewReady);
	}
}

void AFCPlayerController_Lobby::ServerRPCSetReady_Implementation(bool bReady)
{
	AFCPlayerState_Lobby* FCPS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(FCPS))
	{
		FCPS->bIsReady = bReady;
	}
}

#pragma endregion