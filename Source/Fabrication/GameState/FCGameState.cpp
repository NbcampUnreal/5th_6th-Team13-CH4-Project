#include "GameState/FCGameState.h"
#include "Net/UnrealNetwork.h"

void AFCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, bGetKey1);
	DOREPLIFETIME(ThisClass, bGetKey2);
	DOREPLIFETIME(ThisClass, bGetKey3);
	DOREPLIFETIME(ThisClass, bCanEscape);

}

void AFCGameState::SetKeyCollected(int32 KeyIndex)
{
	if (!HasAuthority())
	{
		return;
	}
	
	switch (KeyIndex)
	{
		case 1:
			bGetKey1 = true;
			break;
	
		case 2:
			bGetKey2 = true;
			break;

		case 3:
			bGetKey3 = true;
			break;
		
		default:
			break;
	}
	
	CheckAllKeysCollected();
}

void AFCGameState::CheckAllKeysCollected()
{
	if (bGetKey1 && bGetKey2 && bGetKey3)
	{
		bCanEscape = true;
	}
}

void AFCGameState::CheckCanEscape()
{
	if (!bCanEscape)
	{
		UE_LOG(LogTemp, Warning, TEXT("키 부족"));
		return;
	}
	
	MatchState = EMatchState::Ending;
}

