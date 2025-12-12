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

		FText InputText = ChatText->GetText();
		FString TrimmedText = InputText.ToString().TrimStartAndEnd();

		AFCPlayerController_Lobby* FCPlayerController = Cast<AFCPlayerController_Lobby>(GetWorld()->GetFirstPlayerController());
		if (!IsValid(FCPlayerController)) return;

		AFCPlayerState_Lobby* FCPlayerState = FCPlayerController->GetPlayerState<AFCPlayerState_Lobby>();
		if (!IsValid(FCPlayerState)) return;

		FString Message = FString::Printf(TEXT("%s : %s"), *FCPlayerState->GetPlayerNickName(), *TrimmedText);
		//FCPlayerController->서버에 메세지 보내는 함수
		
		FInputModeGameOnly InputMode;
		FCPlayerController->SetInputMode(InputMode);

		ChatText->SetText(FText::GetEmpty());
		ChatText->SetIsEnabled(false);
	}
}