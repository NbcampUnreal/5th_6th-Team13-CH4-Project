#include "GameState/FCGameState_Lobby.h"

#include "Controller/FCPlayerController_Lobby.h"
#include "GameFramework/PlayerController.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "Net/UnrealNetwork.h"

void AFCGameState_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	DOREPLIFETIME(AFCGameState_Lobby, bAllPlayersReady);
}

bool AFCGameState_Lobby::IsAllPlayersReady() const
{
	if (PlayerArray.Num() == 0) return false;
    
	for (APlayerState* PS : PlayerArray)
	{
		if (AFCPlayerState_Lobby* LobbyPS = Cast<AFCPlayerState_Lobby>(PS))
		{
			if (!LobbyPS->IsReady())
			{
				return false;
			}
		}
	}
	return true;
}

void AFCGameState_Lobby::CheckAllPlayersReady()
{
	if (HasAuthority())
	{
		bool bAllReady = IsAllPlayersReady();
		if (bAllReady != bAllPlayersReady)
		{
			bAllPlayersReady = bAllReady;
		}
	}
}