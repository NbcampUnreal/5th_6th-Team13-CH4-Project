#include "GameState/FCGameState_Lobby.h"

#include "GameFramework/PlayerController.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "Net/UnrealNetwork.h"

void AFCGameState_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	DOREPLIFETIME(AFCGameState_Lobby, RoomHost);
	DOREPLIFETIME(AFCGameState_Lobby, bAllPlayersReady);
	DOREPLIFETIME(AFCGameState_Lobby, RoomList);
}

#pragma region RoomList

void AFCGameState_Lobby::OnRep_RoomList()
{

}

#pragma endregion

bool AFCGameState_Lobby::IsRoomHost(APlayerController* PC) const
{
	return RoomHost == PC;
}

void AFCGameState_Lobby::SetRoomHost(APlayerController* PC)
{
	if (HasAuthority())
	{
		RoomHost = PC;
		bAllPlayersReady = false;
        
		// 모든 플레이어 준비 상태 초기화
		for (APlayerState* PS : PlayerArray)
		{
			if (AFCPlayerState_Lobby* LobbyPS = Cast<AFCPlayerState_Lobby>(PS))
			{
				LobbyPS->SetReady(false);
			}
		}
	}
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

void AFCGameState_Lobby::OnRep_AllPlayersReady()
{
	if (bAllPlayersReady)
	{
		
	}
	else
	{
		
	}
}