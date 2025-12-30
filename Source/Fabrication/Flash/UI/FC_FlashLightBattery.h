#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "FC_FlashLightBattery.generated.h"

UCLASS()
class FABRICATION_API UFC_FlashLightBattery : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* BatteryFillImg;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* NoiseImg;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* BatteryPercentText; 

protected:
	virtual void NativeConstruct() override; 
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* FillMID = nullptr; 

	void UpdateBatteryVisual(float BatteryPercent);
};
