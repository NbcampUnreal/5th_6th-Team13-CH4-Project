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
			// MatchState에 따라 적절한 시간 가져오기
			int32 InitialTime = 0;
			if (GameState->MatchState == EMatchState::Waiting)
			{
				InitialTime = GameState->GetRemainWaitingTime();
			}
			else if (GameState->MatchState == EMatchState::Playing)
			{
				InitialTime = GameState->GetRemainGameTime();
			}
			UpdateTimerText(InitialTime);
			LastRemainGameTime = InitialTime;
			LastMatchState = GameState->MatchState;
		}
		
		// GameState를 찾지 못했어도 타이머는 설정 (UpdateTimer에서 재시도)
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
		// MatchState에 따라 적절한 시간 가져오기
		int32 InitialTime = 0;
		if (GameState->MatchState == EMatchState::Waiting)
		{
			InitialTime = GameState->GetRemainWaitingTime();
		}
		else if (GameState->MatchState == EMatchState::Playing)
		{
			InitialTime = GameState->GetRemainGameTime();
		}
		LastRemainGameTime = InitialTime;
		LastMatchState = GameState->MatchState;
		UpdateTimerText(InitialTime);
		return;
	}

	// MatchState가 변경되었는지 확인
	bool bMatchStateChanged = (GameState->MatchState != LastMatchState);
	
	// MatchState에 따라 적절한 시간 가져오기
	int32 CurrentRemainTime = 0;
	if (GameState->MatchState == EMatchState::Waiting)
	{
		CurrentRemainTime = GameState->GetRemainWaitingTime();
	}
	else if (GameState->MatchState == EMatchState::Playing)
	{
		CurrentRemainTime = GameState->GetRemainGameTime();
	}
	
	// MatchState가 변경되었거나 시간 값이 변경되었을 때 업데이트
	if (bMatchStateChanged || CurrentRemainTime != LastRemainGameTime)
	{
		UpdateTimerText(CurrentRemainTime);
		LastRemainGameTime = CurrentRemainTime;
		LastMatchState = GameState->MatchState;
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

