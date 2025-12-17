#include "GameState/FCGameState.h"
#include "Net/UnrealNetwork.h"
#include "Data/FCKeyType.h"

void AFCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, bGetKey1);
	DOREPLIFETIME(ThisClass, bGetKey2);
	DOREPLIFETIME(ThisClass, bGetKey3);
	DOREPLIFETIME(ThisClass, bCanEscape);
	DOREPLIFETIME(ThisClass, KeyIndex);

}

void AFCGameState::OnRep_OnKeyCollected()
{
	UE_LOG(LogTemp, Error, TEXT("OnRepKey %d"), KeyIndex);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("OnRepKey %d"), KeyIndex));
}

void AFCGameState::SetKeyCollected()
{
	if (!HasAuthority())
	{
		return;
	}
	
	UE_LOG(LogTemp, Error, TEXT("GetKey %d"), KeyIndex)
	++KeyIndex;

	if (KeyIndex >= RequiredKey)
	{
		bCanEscape = true;
		UE_LOG(LogTemp, Error, TEXT("CanEscapse"))
	}
	/*
	switch (KeyID)
	{
	case EFCKeyType::Key_Red:
		bGetKey1 = true;
		break;
	
	case EFCKeyType::Key_Blue:
		bGetKey2 = true;
		break;

	case EFCKeyType::Key_Green:
		bGetKey3 = true;
		break;
		
	default:
		break;
	}
	
	CheckAllKeysCollected();
	*/
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

