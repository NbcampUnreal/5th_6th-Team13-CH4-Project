#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "FC_NoteWidget.generated.h"

UCLASS()
class FABRICATION_API UFC_NoteWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Note_Text;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Note_Title;

	UPROPERTY(meta = (BindWidget))
	UImage* Note_Image;

	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Close; 

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCloseClicked();

	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
