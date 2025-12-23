#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FC_FlashLightBattery.generated.h"

UCLASS()
class FABRICATION_API UFC_FlashLightBattery : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* BatteryProgressBar;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* BatteryPercentText;

public:
	UFUNCTION()
	virtual void NativeConstruct() override; 

	UFUNCTION()
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
