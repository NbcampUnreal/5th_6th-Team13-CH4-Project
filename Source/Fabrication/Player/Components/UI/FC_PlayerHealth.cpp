#include "Player/Components/UI/FC_PlayerHealth.h"
#include "Controller/FCPlayerController.h"
#include "Player/FCPlayerCharacter.h"
#include "Player/Components/StatusComponent.h"
#include "Animation/WidgetAnimation.h"

void UFC_PlayerHealth::NativeConstruct()
{
	Super::NativeConstruct();

	if (Image_Health_1 && CandleOn) Image_Health_1->SetBrushFromTexture(CandleOn);
	
	if (Image_Health_2 && CandleOn) Image_Health_2->SetBrushFromTexture(CandleOn);
	
	if (Image_Health_3 && CandleOn) Image_Health_3->SetBrushFromTexture(CandleOn);
	

	TArray<UImage*> SmokeImages = { Image_Smoke_1, Image_Smoke_2, Image_Smoke_3 };
	for (UImage* SmokeImage : SmokeImages)
	{
		if (SmokeImage)
		{
			SmokeImage->SetVisibility(ESlateVisibility::Hidden);
			SmokeImage->SetRenderOpacity(0.f);
			SmokeImage->SetRenderScale(FVector2D(1.f, 1.f));
		}
	}

	UpdateHealth();
}

void UFC_PlayerHealth::UpdateHealth()
{
	AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwningPlayer());
	if (!PC) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(PC->GetPawn());
	if (!Player || !Player->StatusComp) return;

	if (!CandleOn || !CandleOff) return;

	int32 CurrentHP = Player->StatusComp->GetCurrentHP();

	switch (CurrentHP)
	{
	case 3:
		//깜빡임 에니메이션 중지 
		if (Anim_Candle_1_Slow && IsAnimationPlaying(Anim_Candle_1_Slow))
			StopAnimation(Anim_Candle_1_Slow);
		if (Anim_Candle_2_Slow && IsAnimationPlaying(Anim_Candle_2_Slow))
			StopAnimation(Anim_Candle_2_Slow);
		if (Anim_Candle_3_Slow && IsAnimationPlaying(Anim_Candle_3_Slow))
			StopAnimation(Anim_Candle_3_Slow);

		if (Image_Health_1) Image_Health_1->SetBrushFromTexture(CandleOn);
		if (Image_Health_2) Image_Health_2->SetBrushFromTexture(CandleOn);
		if (Image_Health_3) Image_Health_3->SetBrushFromTexture(CandleOn);
		break;
	case 2:
		if (Image_Health_1) Image_Health_1->SetBrushFromTexture(CandleOn);
		if (Image_Health_2) Image_Health_2->SetBrushFromTexture(CandleOn);
		if (Image_Health_3) Image_Health_3->SetBrushFromTexture(CandleOff);
		//연기 재생 
		if (CurrentHP < LastKnownHP)
		{
			PlaySmokeAnimation(3);
			// 깜빡임 시작
			PlayFlickerAnimation(3);
		}
		break;
	case 1:
		if (Image_Health_1) Image_Health_1->SetBrushFromTexture(CandleOn);
		if (Image_Health_2) Image_Health_2->SetBrushFromTexture(CandleOff);
		if (Image_Health_3) Image_Health_3->SetBrushFromTexture(CandleOff);
		///연기 재생 
		if (CurrentHP < LastKnownHP)
		{
			PlaySmokeAnimation(2);
			// 깜빡임 시작 (2, 3)
			PlayFlickerAnimation(2);
			PlayFlickerAnimation(3);
		}
		break;
	case 0:
		if (Image_Health_1) Image_Health_1->SetBrushFromTexture(CandleOff);
		if (Image_Health_2) Image_Health_2->SetBrushFromTexture(CandleOff);
		if (Image_Health_3) Image_Health_3->SetBrushFromTexture(CandleOff);

		if (CurrentHP < LastKnownHP)
		{
			PlaySmokeAnimation(1);
			// 깜빡임 시작 (1, 2, 3)
			PlayFlickerAnimation(1);
			PlayFlickerAnimation(2);
			PlayFlickerAnimation(3);
		}
		break;
	default:
		break;
	}

	LastKnownHP = CurrentHP;
	Player->UpdateSpeedByHP(CurrentHP);
}

void UFC_PlayerHealth::PlayFlickerAnimation(int32 FlickerIndex)
{
	UWidgetAnimation* AnimToPlay = nullptr;

	switch (FlickerIndex)
	{
	case 1:
		AnimToPlay = Anim_Candle_1_Slow;
		break;
	case 2:
		AnimToPlay = Anim_Candle_2_Slow;
		break;
	case 3:
		AnimToPlay = Anim_Candle_3_Slow;
		break;
	default:
		return;
	}

	if (AnimToPlay)
	{
		StopAnimation(AnimToPlay);
		if (!IsAnimationPlaying(AnimToPlay))
		{
			PlayAnimation(AnimToPlay, 0.f, 1); // 무한 루프
		}
	}
}

void UFC_PlayerHealth::PlaySmokeAnimation(int32 SmokeIndex)
{
	UWidgetAnimation* SmokeAnim = nullptr;
	UImage* SmokeImage = nullptr;

	switch (SmokeIndex)
	{
	case 1:
		SmokeAnim = Anim_Smoke_1;
		SmokeImage = Image_Smoke_1;
		break;
	case 2:
		SmokeAnim = Anim_Smoke_2;
		SmokeImage = Image_Smoke_2;
		break;
	case 3:
		SmokeAnim = Anim_Smoke_3;
		SmokeImage = Image_Smoke_3;
		break;
	default:
		return;
	}

	if (SmokeAnim && SmokeImage)
	{
		SmokeImage->SetVisibility(ESlateVisibility::HitTestInvisible);

		// 연기 애니메이션 1회 재생
		PlayAnimation(SmokeAnim, 0.f, 1);
	}
}