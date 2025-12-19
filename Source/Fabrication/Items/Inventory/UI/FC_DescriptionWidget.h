#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "FC_DescriptionWidget.generated.h"


UCLASS()
class FABRICATION_API UFC_DescriptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* DescriptionText;
	
	UFUNCTION(BlueprintCallable)
	void SetDescriptionText(const FText& text);
};
