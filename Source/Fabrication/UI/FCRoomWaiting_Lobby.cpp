#include "UI/FCRoomWaiting_Lobby.h"
#include "Components/TextBlock.h"

void UFCRoomWaiting_Lobby::SetRoomTitle(const FText& InRoomName)
{
	if (!IsValid(RoomName)) return;
	// 방이름 표시용 함수
}

void UFCRoomWaiting_Lobby::UpdatePlayerList(const FText& HostName, const TArray<FText>& GuestNames)
{
	// 플레이어 목록 표시용 함수 빈 슬롯은 Waiting 으로 SetText
}

void UFCRoomWaiting_Lobby::SetPlayButtonText(const FText& InText)
{
	// 방장은 Play 표시, 나머지는 Ready 표시
}
