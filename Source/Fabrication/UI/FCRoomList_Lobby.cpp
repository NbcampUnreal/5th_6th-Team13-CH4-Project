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

void UFCRoomList_Lobby::AddRoomEntry(const FText& RoomTitle)
{
	if (!IsValid(RoomListScroll)) return;
	// 생성된 방이름 표시
}
