#include "GameInstance/FCGameInstance.h"

void UFCGameInstance::SetLocalPlayerNickName(const FString& NewName)
{
	LocalPlayerNickName = NewName;
	UE_LOG(LogTemp, Error, TEXT("%s"), *GetLocalPlayerNickName())
}

const FString& UFCGameInstance::GetLocalPlayerNickName() const
{
	return LocalPlayerNickName;
}
