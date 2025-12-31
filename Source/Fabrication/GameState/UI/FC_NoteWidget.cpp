#include "GameState/UI/FC_NoteWidget.h"
#include "Controller/FCPlayerController.h"

void UFC_NoteWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(BTN_Close)
	{
		BTN_Close->OnClicked.AddUniqueDynamic(this, &UFC_NoteWidget::OnCloseClicked);
	}
	SetIsFocusable(true);

	if (Anim_Close)
	{
		FWidgetAnimationDynamicEvent Event;
		Event.BindUFunction(this, FName("OnCloseAnimFinished"));
		BindToAnimationFinished(Anim_Close, Event);
	}
}

void UFC_NoteWidget::PlayOpen()
{
	bClosing = false; 

	SetVisibility(ESlateVisibility::Visible);
	SetIsEnabled(true);
	UE_LOG(LogTemp, Error, TEXT("PlayOpen called. Anim_Open=%s"),
		Anim_Open ? TEXT("VALID") : TEXT("NULL"));
	if (Anim_Open)
	{
		PlayAnimation(Anim_Open, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
}

void UFC_NoteWidget::PlayClose()
{
	if (bClosing) return;
	bClosing = true; 

	SetIsEnabled(false);

	if (Anim_Close)
		PlayAnimation(Anim_Close, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	else
		OnCloseAnimFinished();
}

void UFC_NoteWidget::OnCloseAnimFinished()
{
	SetVisibility(ESlateVisibility::Collapsed);
	bClosing = false;

	if (AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwningPlayer()))
	{
		PC->CloseNote();
	}
}

void UFC_NoteWidget::OnCloseClicked()
{
	PlayClose();
}

FReply UFC_NoteWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if(InKeyEvent.GetKey() == EKeys::E)
	{
		PlayClose();
		return FReply::Handled();
	}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UFC_NoteWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SetKeyboardFocus();
	return FReply::Handled();
}
