#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCTimerWidget.generated.h"

class UTextBlock;

UCLASS()
class FABRICATION_API UFCTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
};
