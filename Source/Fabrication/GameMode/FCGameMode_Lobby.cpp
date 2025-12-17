#include "GameMode/FCGameMode_Lobby.h"
#include "GameState/FCGameState_Lobby.h"
#include "Controller/FCPlayerController_Lobby.h"
#include "PlayerState/FCPlayerState_Lobby.h"

AFCGameMode_Lobby::AFCGameMode_Lobby()
{
	GameStateClass = AFCGameState_Lobby::StaticClass();
	PlayerControllerClass = AFCPlayerController_Lobby::StaticClass();
	PlayerStateClass = AFCPlayerState_Lobby::StaticClass();
}

void AFCGameMode_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllers.Add(NewPlayer);

}

void AFCGameMode_Lobby::SendChatMessage(const FString& Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AFCPlayerController_Lobby* PC_Lobby = Cast<AFCPlayerController_Lobby>(*It);
		if (IsValid(PC_Lobby))
		{
			PC_Lobby->ClientRPCAddChatMessage(Message);
		}
	}
}

