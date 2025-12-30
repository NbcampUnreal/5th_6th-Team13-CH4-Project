#include "Flash/UI/FC_FlashLightBattery.h"
#include "Controller/FCPlayerController.h"
#include "Player/FCPlayerCharacter.h"

void UFC_FlashLightBattery::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BatteryFillImg)
	{
		FillMID = BatteryFillImg->GetDynamicMaterial();
		// GetDynamicMaterial() => Brush에 머티리얼이 들어있을 때만 동작
	}

	//초기값 설정 
	UpdateBatteryVisual(1.0f);
}

void UFC_FlashLightBattery::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwningPlayer());
	if (!PC) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(PC->GetPawn());
	if (!Player) return;

	const float BatteryPercent = Player->GetBatteryPercent();
	UpdateBatteryVisual(BatteryPercent);
}

void UFC_FlashLightBattery::UpdateBatteryVisual(float BatteryPercent)
{
	BatteryPercent = FMath::Clamp(BatteryPercent, 0.0f, 1.0f);

	//Material Parameter를 통해 배터리 사용 업데이트
	if (FillMID)
	{
		FillMID->SetScalarParameterValue(TEXT("Battery"), BatteryPercent);

		FLinearColor StateColor = FLinearColor::Green;
		if (BatteryPercent <= 0.2f) StateColor = FLinearColor::Red;
		else if (BatteryPercent <= 0.5f) StateColor = FLinearColor::Yellow;

		FillMID->SetVectorParameterValue(TEXT("StateColor"), StateColor);

		//30% 이하 깜빡임 강도
		const float Flicker = (BatteryPercent <= 0.3f) ? 1.0f : 0.0f;
		FillMID->SetScalarParameterValue(TEXT("FlickerOn"), Flicker);
	}

	if (BatteryPercentText)
	{
		const FString PercentStr = FString::Printf(TEXT("%.0f%%"), BatteryPercent * 100.0f);
		BatteryPercentText->SetText(FText::FromString(PercentStr));
	}
}
