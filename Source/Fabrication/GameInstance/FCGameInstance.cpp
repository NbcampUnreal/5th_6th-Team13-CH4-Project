#include "GameInstance/FCGameInstance.h"

void UFCGameInstance::SetPlayerNickName(const FString& NewName)
{
	PlayernNickName = NewName;
}

const FString& UFCGameInstance::GetPlayerNickName() const
{
	return PlayernNickName;
}
