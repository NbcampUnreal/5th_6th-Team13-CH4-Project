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

// 로비에서 자동으로 게임맵으로 이동하는 테스트용 로직
void AFCGameMode_Lobby::TravelToGameMap()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("로비 맵에서 게임 맵으로 이동: %s"), *GameMapPath);
		GetWorld()->ServerTravel(GameMapPath + TEXT("?listen"));
	}
}

void AFCGameMode_Lobby::BeginPlay()
{
	Super::BeginPlay();
	
	// 테스트용: AutoTravelDelay가 0보다 크면 자동으로 게임 맵으로 이동
	if (AutoTravelDelay > 0.0f && HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			AutoTravelTimerHandle,
			this,
			&AFCGameMode_Lobby::TravelToGameMap,
			AutoTravelDelay,
			false
		);
		UE_LOG(LogTemp, Warning, TEXT("로비 맵: %.1f초 후 자동으로 게임 맵으로 이동합니다."), AutoTravelDelay);
	}
}
