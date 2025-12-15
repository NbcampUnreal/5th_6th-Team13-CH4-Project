#include "PlayerState/FCPlayerState.h"
#include "Net/UnrealNetwork.h"

void AFCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bIsReady);
	DOREPLIFETIME(ThisClass, PlayerNickName);
}

void AFCPlayerState::OnRep_ChangedPlayerNickName()
{
	UE_LOG(LogTemp, Warning, TEXT("ChangedPlayerNickName: %s"), *PlayerNickName);
}

void AFCPlayerState::SetPlayerNickName(const FString& NewNickName)
{
	PlayerNickName = NewNickName;
	UE_LOG(LogTemp, Warning, TEXT("PlayerNickName: %s"), *PlayerNickName);
}
