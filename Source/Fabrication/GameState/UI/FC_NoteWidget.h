#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "FC_NoteWidget.generated.h"

class UWidgetAnimation;

UCLASS()
class FABRICATION_API UFC_NoteWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Note_Text;

	UPROPERTY(meta = (BindWidget))
	UImage* Note_Image;

	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Close; 

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* Anim_Open;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* Anim_Close;

public:
	UFUNCTION()
	void OnCloseClicked();

	UFUNCTION()
	void PlayOpen();

	UFUNCTION()
	void PlayClose();

	UFUNCTION()
	void OnCloseAnimFinished();
	
private:
	bool bClosing = false; 
};
