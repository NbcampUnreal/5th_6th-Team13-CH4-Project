#include "PlayerState/FCPlayerState_Lobby.h"
#include "GameMode/FCGameMode_Lobby.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AFCPlayerState_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, PlayerNickName);

}

void AFCPlayerState_Lobby::SetPlayerNickName(const FString& InNickName)
{
	PlayerNickName = InNickName;
}

const FString& AFCPlayerState_Lobby::GetPlayerNickName() const
{
	return PlayerNickName;
}

void AFCPlayerState_Lobby::OnRep_PlayerNickName()
{
	if (HasAuthority())
	{
		if (AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>())
		{
			GM->MulticastLogNickName(PlayerNickName);
		}
	}
}
