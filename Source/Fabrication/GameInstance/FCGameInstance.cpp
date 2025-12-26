#include "GameInstance/FCGameInstance.h"

UFCGameInstance::UFCGameInstance()
{
	// 생성자에서 초기화
}

void UFCGameInstance::SetLocalPlayerNickName(const FString& NewName)
{
	LocalPlayerNickName = NewName;
	UE_LOG(LogTemp, Error, TEXT("%s"), *GetLocalPlayerNickName())
}

const FString& UFCGameInstance::GetLocalPlayerNickName() const
{
	return LocalPlayerNickName;
}
