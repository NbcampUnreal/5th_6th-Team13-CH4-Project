#include "UI/FCResultWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "GameState/FCGameState.h"

void UFCResultWidget::SetResultImage(bool bIsEscaped)
{
	if (!IsValid(ResultImage))
	{
		return;
	}
	
	if (bIsEscaped)
	{
		// 탈출 성공 이미지 설정
		if (IsValid(EscapeSuccessTexture))
		{
			ResultImage->SetBrushFromTexture(EscapeSuccessTexture);
		}
	}
	else
	{
		// 탈출 실패 이미지 설정
		if (IsValid(EscapeFailedTexture))
		{
			ResultImage->SetBrushFromTexture(EscapeFailedTexture);
		}
	}
}

void UFCResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AGameStateBase* GS = GetWorld()->GetGameState();

	if (IsValid(GS))
	{
		AFCGameState* FCGS = Cast<AFCGameState>(GS);
		
		int32 PlayTime = FCGS->GetPlayGameTime();
		
		int32 Minutes = PlayTime / 60;
		int32 Seconds = PlayTime % 60;
	
		FString TimeString = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		TimerText->SetText(FText::FromString(TimeString));
	}
}
