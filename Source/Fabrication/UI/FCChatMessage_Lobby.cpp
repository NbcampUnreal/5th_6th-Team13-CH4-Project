#include "UI/FCChatMessage_Lobby.h"
#include "Components/TextBlock.h"

void UFCChatMessage_Lobby::SetChatMessage(const FString& Message)
{
	if (IsValid(ChatMessageTextBlock))
	{
		ChatMessageTextBlock->SetText(FText::FromString(Message));
		ChatMessageTextBlock->Font.Size = 16;
	}
	
}
