#include "UI/FCRoomList_Lobby.h"
#include "Components/TextBlock.h"

void UFCRoomList_Lobby::SetPlayerNickNameText(const FString& InNickName)
{
	if (IsValid(ViewPlayerNickName))
	{
		ViewPlayerNickName->SetText(FText::FromString(InNickName));
	}
}
