#include "PlayerState/FCPlayerState.h"
#include "Net/UnrealNetwork.h"

void AFCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bIsReady);
	DOREPLIFETIME(ThisClass, bIsDead);
}
