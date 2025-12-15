#include "UI/FCChatting_Lobby.h"
#include "Controller/FCPlayerController_Lobby.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"

void UFCChatting_Lobby::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ChatText))
	{
		ChatText->OnTextCommitted.AddDynamic(this, &UFCChatting_Lobby::OnTextCommitted);
	}

	ChatText->SetIsEnabled(false);
}

void UFCChatting_Lobby::ActivateChatText()
{
	if (IsValid(ChatText))
	{
		ChatText->SetIsEnabled(true);
		ChatText->SetFocus();
	}
}

void UFCChatting_Lobby::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (!IsValid(ChatText)) return;

		AFCPlayerController_Lobby* PC = Cast<AFCPlayerController_Lobby>(GetWorld()->GetFirstPlayerController());
		if (!IsValid(PC)) return;

		AFCPlayerState_Lobby* PS = PC->GetPlayerState<AFCPlayerState_Lobby>();
		if (!IsValid(PS)) return;

		FText InputText = ChatText->GetText();
		FString TrimmedText = InputText.ToString().TrimStartAndEnd();
		FString Message = FString::Printf(TEXT("%s : %s"), *PS->GetPlayerNickName(), *TrimmedText);

		PC->ServerRPCSendChatMessage(Message);
		
		ChatText->SetText(FText::GetEmpty());
		//ChatText->SetIsEnabled(false);
	}
}