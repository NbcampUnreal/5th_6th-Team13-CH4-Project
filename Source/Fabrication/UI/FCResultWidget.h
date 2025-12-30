#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCResultWidget.generated.h"

class UTextBlock;

UCLASS()
class FABRICATION_API UFCResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
	
public:
	void SetResultText(const FString& Result);
};