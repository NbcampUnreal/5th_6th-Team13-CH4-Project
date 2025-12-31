#include "UI/FCTimerWidget.h"
#include "Components/TextBlock.h"
#include "GameState/FCGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UFCTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// GameState 가져오기 (실패해도 타이머는 설정 - UpdateTimer에서 재시도)
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		GameState = World->GetGameState<AFCGameState>();
		
		// GameState를 찾았으면 초기화
		if (IsValid(GameState))
		{
			// 초기 타이머 값 설정
			UpdateTimerText(GameState->GetRemainGameTime());
			LastRemainGameTime = GameState->GetRemainGameTime();
		}
		
		// GameState를 찾지 못했어도 타이머는 설정 (UpdateTimer에서 재시도)
		// 주기적으로 타이머 업데이트 (0.1초마다)
		World->GetTimerManager().SetTimer(
			TimerUpdateHandle,
			this,
			&UFCTimerWidget::UpdateTimer,
			0.1f,
			true
		);
	}
}

void UFCTimerWidget::NativeDestruct()
{
	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerUpdateHandle);
	}

	Super::NativeDestruct();
}

void UFCTimerWidget::UpdateTimer()
{
	if (!IsValid(GameState))
	{
		// GameState가 유효하지 않으면 다시 가져오기
		UWorld* World = GetWorld();
		if (IsValid(World))
		{
			GameState = World->GetGameState<AFCGameState>();
		}
		
		// 여전히 찾지 못했으면 다음 프레임에 재시도
		if (!IsValid(GameState))
		{
			return;
		}
		
		// GameState를 찾았으면 초기화
		LastRemainGameTime = GameState->GetRemainGameTime();
		UpdateTimerText(LastRemainGameTime);
	}

	int32 CurrentRemainTime = GameState->GetRemainGameTime();
	
	// 값이 변경되었을 때만 업데이트
	if (CurrentRemainTime != LastRemainGameTime)
	{
		UpdateTimerText(CurrentRemainTime);
		LastRemainGameTime = CurrentRemainTime;
	}
}

void UFCTimerWidget::UpdateTimerText(int32 RemainTime)
{
	if (!IsValid(TimerText))
	{
		return;
	}

	// 분:초 형식으로 변환
	int32 Minutes = RemainTime / 60;
	int32 Seconds = RemainTime % 60;
	
	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	TimerText->SetText(FText::FromString(TimeString));
}

