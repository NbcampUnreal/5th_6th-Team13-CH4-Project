#include "Controller/FCPlayerController_Lobby.h"
#include "GameInstance/FCGameInstance.h"
#include "PlayerState/FCPlayerState_Lobby.h"

void AFCPlayerController_Lobby::ServerSendPlayerNickName_Implementation(const FString& InNickName)
{
	AFCPlayerState_Lobby* FCPlayerState = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(FCPlayerState))
	{
		FCPlayerState->SetPlayerNickName(InNickName);
	}
}

void AFCPlayerController_Lobby::ServerSendChatMessage_Implementation(const FString& Message)
{
}

void AFCPlayerController_Lobby::ClientAddChatMessage_Implementation(const FString& Message)
{
}

void AFCPlayerController_Lobby::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		UFCGameInstance* FCGameInstance = GetGameInstance<UFCGameInstance>();
		if (IsValid(FCGameInstance))
		{
			ServerSendPlayerNickName(FCGameInstance->GetPlayerNickName());
		}
	}
	
}
