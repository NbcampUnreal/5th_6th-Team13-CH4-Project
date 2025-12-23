#include "UI/FCHUD_Lobby.h"
#include "UI/FCChatting_Lobby.h"
#include "UI/FCRoomList_Lobby.h"
#include "UI/FCRoomWaiting_Lobby.h"

void UFCHUD_Lobby::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
	ManagedWidgets.Empty();

	if (IsValid(Chatting))
	{
		ManagedWidgets.Add(Chatting);
	}

	if (IsValid(RoomList))
	{
		ManagedWidgets.Add(RoomList);
	}

	if (IsValid(RoomWaiting))
	{
		ManagedWidgets.Add(RoomWaiting);
	}

}

FReply UFCHUD_Lobby::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		if (IsValid(Chatting))
		{
			if (Chatting->HandleEnterKey())
			{
				return FReply::Handled();
			}
		}
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
