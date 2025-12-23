#include "Player/Components/UI/FC_PlayerHealth.h"
#include "Controller/FCPlayerController.h"
#include "Player/FCPlayerCharacter.h"
#include "Player/Components/StatusComponent.h"

void UFC_PlayerHealth::NativeConstruct()
{
	if (Image_Health_1 && CandleOn)
	{
		Image_Health_1->SetBrushFromTexture(CandleOn);
	}
	if (Image_Health_2 && CandleOn)
	{
		Image_Health_2->SetBrushFromTexture(CandleOn);
	}
	if (Image_Health_3 && CandleOn)
	{
		Image_Health_3->SetBrushFromTexture(CandleOn);
	}
}

void UFC_PlayerHealth::UpdateHealth()
{
	AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwningPlayer());
	if (!PC) return;
	if (!PC->HealthWidgetInstance) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(PC->GetPawn());
	if (!Player) return;

	if (!CandleOn || !CandleOff) return;
	
	switch (Player->StatusComp->GetCurrentHP())
	{
	case 3:
		if (Image_Health_1) Image_Health_1->SetBrushFromTexture(CandleOn);
		if (Image_Health_2) Image_Health_2->SetBrushFromTexture(CandleOn);
		if (Image_Health_3) Image_Health_3->SetBrushFromTexture(CandleOn);
		break;
	case 2:
		if (Image_Health_1) Image_Health_1->SetBrushFromTexture(CandleOn);
		if (Image_Health_2) Image_Health_2->SetBrushFromTexture(CandleOn);
		if (Image_Health_3) Image_Health_3->SetBrushFromTexture(CandleOff);
		break;
	case 1:
		if (Image_Health_1) Image_Health_1->SetBrushFromTexture(CandleOn);
		if (Image_Health_2) Image_Health_2->SetBrushFromTexture(CandleLow);
		if (Image_Health_3) Image_Health_3->SetBrushFromTexture(CandleOff);
		break;
	case 0:
		if (Image_Health_1) Image_Health_1->SetBrushFromTexture(CandleLow);
		if (Image_Health_2) Image_Health_2->SetBrushFromTexture(CandleOff);
		if (Image_Health_3) Image_Health_3->SetBrushFromTexture(CandleOff);
	default:
		break;
	}
}