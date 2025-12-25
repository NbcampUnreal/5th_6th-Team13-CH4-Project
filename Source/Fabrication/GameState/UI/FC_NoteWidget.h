#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "FC_NoteWidget.generated.h"

UCLASS()
class FABRICATION_API UFC_NoteWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Note_Text;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title_Text;

	UPROPERTY(meta = (BindWidget))
	UImage* Note_Image;

public:
	virtual void NativeConstruct() override; 
	UFUNCTION()
	void UpdateNoteUI();
};
