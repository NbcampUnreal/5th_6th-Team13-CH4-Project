#include "GameInstance/FCGameInstance.h"

void UFCGameInstance::SetPlayerNickName(const FString& NewName)
{
	PlayernNickName = NewName;
	UE_LOG(LogTemp, Error, TEXT("%s"), *GetPlayerNickName())
}

const FString& UFCGameInstance::GetPlayerNickName() const
{
	return PlayernNickName;
}
