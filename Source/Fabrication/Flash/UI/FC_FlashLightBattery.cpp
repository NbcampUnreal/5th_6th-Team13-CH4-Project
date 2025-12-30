#include "Flash/UI/FC_FlashLightBattery.h"
#include "Controller/FCPlayerController.h"
#include "Player/FCPlayerCharacter.h"
#include "Flash/FlashLight.h"
#include "Components/SpotLightComponent.h"

void UFC_FlashLightBattery::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BatteryFillImg && BatteryFillImg->GetBrush().GetResourceObject())
	{
		// 원본 머티리얼을 가져와서 MID 생성
		UObject* Resource = BatteryFillImg->GetBrush().GetResourceObject();
		if (UMaterialInterface* Material = Cast<UMaterialInterface>(Resource))
		{
			FillMID = UMaterialInstanceDynamic::Create(Material, this);

			if (FillMID)
			{
				// 새로 생성한 MID를 Image에 적용
				BatteryFillImg->SetBrushFromMaterial(FillMID);
			}
		}
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
	AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwningPlayer());
	if (!PC) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(PC->GetPawn());
	if (!Player) return;

	BatteryPercent = FMath::Clamp(BatteryPercent, 0.0f, 1.0f);

	//Material Parameter를 통해 배터리 사용 업데이트
	if (FillMID)
	{
		FillMID->SetScalarParameterValue(TEXT("Battery"), BatteryPercent);

		FLinearColor StateColor = FLinearColor(0.10f, 0.35f, 0.18f, 1.0f);
		float LightIntensity = 5000.0f;

		if (BatteryPercent <= 0.2f)
		{
			StateColor = FLinearColor(0.45f, 0.08f, 0.06f, 1.0f);
			LightIntensity = 1000.0f;
		}
		else if (BatteryPercent <= 0.5f)
		{
			StateColor = FLinearColor(0.55f, 0.45f, 0.12f, 1.0f);
			LightIntensity = 2500.0f;
		}
		else
		{
			StateColor = FLinearColor(0.10f, 0.35f, 0.18f, 1.0f);
			LightIntensity = 5000.0f;
		}

		if (Player->FlashLightInstance && Player->FlashLightInstance->SpotLight)
		{
			Player->FlashLightInstance->SpotLight->SetIntensity(LightIntensity);
		}

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
