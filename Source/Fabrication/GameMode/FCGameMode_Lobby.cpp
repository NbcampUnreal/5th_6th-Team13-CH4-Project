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

void AFCGameMode_Lobby::MulticastLogNickName_Implementation(const FString& InNickName)
{
	UE_LOG(LogTemp, Error, TEXT("클라이언트 Log: 플레이어 접속 %s"), *InNickName);
}
