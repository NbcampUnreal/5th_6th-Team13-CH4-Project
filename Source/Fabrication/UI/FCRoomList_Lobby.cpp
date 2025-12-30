#include "UI/FCRoomList_Lobby.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"

void UFCRoomList_Lobby::SetPlayerNickNameText(const FText& InNickName)
{
	if (IsValid(ViewPlayerNickName))
	{
		ViewPlayerNickName->SetText(InNickName);
	}
}