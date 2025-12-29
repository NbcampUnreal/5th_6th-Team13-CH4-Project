#include "GameState/UI/FC_NoteWidget.h"
#include "Controller/FCPlayerController.h"

void UFC_NoteWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(BTN_Close)
	{
		BTN_Close->OnClicked.AddUniqueDynamic(this, &UFC_NoteWidget::OnCloseClicked);
	}
}

void UFC_NoteWidget::OnCloseClicked()
{
	if(AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwningPlayer()))
	{
		PC->CloseNote();
	}
}

FReply UFC_NoteWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if(InKeyEvent.GetKey() == EKeys::E)
	{
		if (AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwningPlayer()))
		{
			PC->CloseNote();
			return FReply::Handled();
		}
	}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UFC_NoteWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SetKeyboardFocus();
	return FReply::Handled();
}
