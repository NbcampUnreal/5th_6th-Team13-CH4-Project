#include "PlayerState/FCPlayerState_Lobby.h"
#include "Net/UnrealNetwork.h"

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