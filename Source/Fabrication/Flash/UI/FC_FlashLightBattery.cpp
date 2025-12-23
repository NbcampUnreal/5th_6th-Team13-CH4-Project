#include "Flash/UI/FC_FlashLightBattery.h"
#include "Controller/FCPlayerController.h"
#include "Player/FCPlayerCharacter.h"

void UFC_FlashLightBattery::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BatteryProgressBar)
	{
		BatteryProgressBar->SetPercent(1.0f);
	}
	if (BatteryPercentText)
	{
		BatteryPercentText->SetText(FText::FromString(TEXT("100%")));
	}
}

void UFC_FlashLightBattery::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwningPlayer());
	if (!PC) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(PC->GetPawn());
	if (!Player) return;

	const float BatteryPercent = Player->GetBatteryPercent();

	if (BatteryProgressBar)
	{
		BatteryProgressBar->SetPercent(BatteryPercent);
		//20%
		if (BatteryPercent <= 0.2f)
		{
			BatteryProgressBar->SetFillColorAndOpacity(FLinearColor::Red);
		}
		//50%
		else if (BatteryPercent <= 0.5f)
		{
			BatteryProgressBar->SetFillColorAndOpacity(FLinearColor::Yellow);
		}
		else
		{
			BatteryProgressBar->SetFillColorAndOpacity(FLinearColor::Green);
		}
	}

	if (BatteryPercentText)
	{
		const FString PercentStr = FString::Printf(TEXT("%.0f%%"), BatteryPercent * 100.0f);
		BatteryPercentText->SetText(FText::FromString(PercentStr));
	}
}
