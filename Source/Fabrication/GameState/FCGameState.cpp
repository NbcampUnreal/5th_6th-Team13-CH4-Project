#include "GameState/FCGameState.h"
#include "Net/UnrealNetwork.h"

void AFCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, bGetFuse1);
	DOREPLIFETIME(ThisClass, bGetFuse2);
	DOREPLIFETIME(ThisClass, bGetFuse3);
	DOREPLIFETIME(ThisClass, bCanEscape);

}

void AFCGameState::SetFuseCollected(int32 FuseIndex)
{
	if (!HasAuthority())
	{
		return;
	}
	
	switch (FuseIndex)
	{
		case 1:
			bGetFuse1 = true;
			break;
	
		case 2:
			bGetFuse2 = true;
			break;

		case 3:
			bGetFuse3 = true;
			break;
		
		default:
			break;
	}
	
	CheckAllFusesCollected();
}

void AFCGameState::CheckAllFusesCollected()
{
	if (bGetFuse1 && bGetFuse2 && bGetFuse3)
	{
		bCanEscape = true;
	}
}
