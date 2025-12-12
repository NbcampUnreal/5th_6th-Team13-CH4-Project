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
