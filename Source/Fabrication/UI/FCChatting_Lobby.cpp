#include "UI/FCChatting_Lobby.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UFCChatting_Lobby::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;

	if (IsValid(ChatText))
	{
		ChatText->OnTextCommitted.AddDynamic(this, &UFCChatting_Lobby::OnTextCommitted);
	}

	ActivateChatText();
}

void UFCChatting_Lobby::SetCurrentRoomName(const FText& InRoomName)
{
	if (!IsValid(RoomName)) return;
	// 현재 방이름 출력
}

void UFCChatting_Lobby::AddSystemMessage(const FText& Message)
{
	FText SystemMessage = FText::Format(FText::FromString(TEXT("[SYSTEM] {0}")), Message);
	AddText(SystemMessage, FLinearColor::Yellow);
	UGameplayStatics::PlaySound2D(this, SystemSound);
}

void UFCChatting_Lobby::AddChatMessage(const FText& Message)
{
	AddText(Message);
	UGameplayStatics::PlaySound2D(this, ChatSound);
}

void UFCChatting_Lobby::ActivateChatText()
{
	if (IsValid(ChatText))
	{
		ChatText->SetIsEnabled(true);
		ChatText->SetKeyboardFocus();
	}
}

void UFCChatting_Lobby::DeactivateChatText()
{
	if (IsValid(ChatText))
	{
		ChatText->SetText(FText::GetEmpty());
		ChatText->SetIsEnabled(false);
		
		TSharedPtr<SWidget> SafeWidget = GetCachedWidget();
		if (SafeWidget.IsValid())
		{
			FSlateApplication::Get().SetAllUserFocus(SafeWidget.ToSharedRef(), EFocusCause::SetDirectly);
		}

		this->SetFocus();
	}
}

void UFCChatting_Lobby::AddText(const FText& Text, FLinearColor Color)
{
	if (!IsValid(ChatScrollBox)) return;

	UTextBlock* TextBlock = NewObject<UTextBlock>(this);
	if (IsValid(TextBlock)) 
	{
		FSlateFontInfo FontInfo = TextBlock->Font;
		FontInfo.Size = 18;

		TextBlock->SetFont(FontInfo);
		TextBlock->SetText(Text);
		TextBlock->SetColorAndOpacity(FSlateColor(Color));
		TextBlock->SetAutoWrapText(true);

		ChatScrollBox->AddChild(TextBlock);
		ChatScrollBox->ScrollToEnd();
		ChatScrollBox->bAnimateWheelScrolling = true;
	}

}

bool UFCChatting_Lobby::HandleEnterKey()
{
	if (!ChatText->GetIsEnabled() || !ChatText->HasKeyboardFocus())
	{
		ActivateChatText();
		return true;
	}

	return false;
}

void UFCChatting_Lobby::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		FText InputText = ChatText->GetText();
		FString TrimmedText = InputText.ToString().TrimStartAndEnd();
		if (!TrimmedText.IsEmpty())
		{
			OnChatCommitted.ExecuteIfBound(TrimmedText);
			ChatText->SetText(FText::GetEmpty());
			ChatText->SetKeyboardFocus();
		}
		else
		{
			DeactivateChatText();
		}
	}
}