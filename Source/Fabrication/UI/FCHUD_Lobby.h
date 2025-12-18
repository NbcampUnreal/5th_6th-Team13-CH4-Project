#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCHUD_Lobby.generated.h"

class UFCChatMessage_Lobby;
class UFCChatting_Lobby;
class UFCRoomList_Lobby;
class UFCRoomWaiting_Lobby;

UCLASS()
class FABRICATION_API UFCHUD_Lobby : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFCChatMessage_Lobby> ChatMessage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFCChatting_Lobby> Chatting;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFCRoomList_Lobby> RoomList;
};
