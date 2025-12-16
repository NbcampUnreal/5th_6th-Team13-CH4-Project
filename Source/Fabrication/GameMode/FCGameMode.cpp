#include "GameMode/FCGameMode.h"

#include "Controller/FCPlayerController.h"
#include "GameState/FCGameState.h"
#include "Event/LevelEventManager.h"
#include "PlayerState/FCPlayerState.h"

AFCGameMode::AFCGameMode()
	:	
	MinimumPlayerCountForPlaying(2),
	WaitingTime(10.0),
	RemainTimeForPlaying(WaitingTime),
	GameTimeLimit(10),
	RemainGameTime(GameTimeLimit),
	bReadyForPlay(false),
	bAllPlayersReady(false),
	EndingTimeLimit(10),
	RemainEndingTime(EndingTimeLimit)
{
}

void AFCGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (ULevelEventManager* Manager = GetWorld()->GetSubsystem<ULevelEventManager>())
	{
		Manager->HazardDataTable = SetHazardDataTable;
		Manager->StartEventLoop(EHazardType::Garden);
	}
	
	GetWorldTimerManager().SetTimer(
		MainTimerHandle,
		this,
		&ThisClass::OnMainTimerElapsed,
		1.f,
		true);
}

void AFCGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AFCGameState* FCGS = GetGameState<AFCGameState>();
	if (!IsValid(FCGS))
	{
		return;
	}
	
	AFCPlayerController* FCPC = Cast<AFCPlayerController>(NewPlayer);
	if (IsValid(FCPC))
	{
		AlivePlayerControllers.Add(FCPC);
	}
}

void AFCGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	AFCPlayerController* FCPC = Cast<AFCPlayerController>(Exiting);
	if (IsValid(FCPC))
	{
		AlivePlayerControllers.Remove(FCPC);
		DeadPlayerControllers.Add(FCPC);
	}
}

void AFCGameMode::OnMainTimerElapsed()
{
	AFCGameState* FCGS = GetGameState<AFCGameState>();
	if (!IsValid(FCGS))
	{
		return;
	}
	
	switch (FCGS->MatchState)
	{
	case EMatchState::None:
		break;
		
	case EMatchState::Waiting:
		UE_LOG(LogTemp, Warning, TEXT("남은시간 : %d"), RemainTimeForPlaying);
		
		if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			// 인원이 부족하다는 메세지 출력 할 곳
			
			bReadyForPlay = false;
		}
		else
		{
			bAllPlayersReady = true;
			
			for (APlayerState* PS : FCGS->PlayerArray)
			{
				AFCPlayerState* FCPS = Cast<AFCPlayerState>(PS);
				
				if (IsValid(FCPS))
				{
					if (!FCPS->bIsReady)
					{
						bAllPlayersReady = false;
						RemainTimeForPlaying = WaitingTime;
						break;
					}
				}
			}
		}
		
		if (bAllPlayersReady)
		{
			bReadyForPlay = true;
			--RemainTimeForPlaying;
			// 게임 시작까지 남은 시간 출력할 곳
		}
		
		if (RemainTimeForPlaying <= 0)
		{
			FCGS->MatchState = EMatchState::Playing;
			
			// GetWorldTimerManager().SetTimer(
			// GameTimeLimitHandle,
			// this,
			// &ThisClass::DecreaseGameTime,
			// 1.f,
			// true);
			
			GetWorld()->ServerTravel(GameMapPath + TEXT("?listen"));
		}
		break;
		
	case EMatchState::Playing:
		UE_LOG(LogTemp, Warning, TEXT("게임 남은 시간 : %d"), RemainGameTime);
		
		--RemainGameTime;
		
		if (AlivePlayerControllers.Num() <= 0 || RemainGameTime <= 0)
		{
			FCGS->MatchState = EMatchState::Ending;
			
			GetWorldTimerManager().ClearTimer(GameTimeLimitHandle);
			
			// GetWorldTimerManager().SetTimer(
			// EndingTimerHandle,
			// this,
			// &ThisClass::DecreaseEndingTime,
			// 1.f,
			// true);
		}
		break;
		
	case EMatchState::Ending:
		UE_LOG(LogTemp, Warning, TEXT("엔딩 남은 시간 : %d"), RemainEndingTime);
		
		--RemainEndingTime;
		
		if (RemainEndingTime <= 0)
		{
			GetWorldTimerManager().ClearTimer(EndingTimerHandle);
			
			GetWorld()->ServerTravel(LobbyMapPath + TEXT("?listen"));
			
			ResetValues();
		}
		break;
	
	case EMatchState::End:
		break;
	
	default:
		break;
	}
}

// void AFCGameMode::DecreaseGameTime()
// {
// 	--RemainGameTime;
// }
//
// void AFCGameMode::DecreaseEndingTime()
// {
// 	--RemainEndingTime;
// }

void AFCGameMode::ResetValues()
{
	AFCGameState* FCGS = GetGameState<AFCGameState>();
	
	for (APlayerState* PS : FCGS->PlayerArray)
	{
		AFCPlayerState* FCPS = Cast<AFCPlayerState>(PS);
		if (IsValid(FCPS))
		{
			FCPS->bIsReady = false;
		}
	}
	
	bReadyForPlay = false;
	bAllPlayersReady = false;
	RemainTimeForPlaying = WaitingTime;
	RemainGameTime = GameTimeLimit;
	RemainEndingTime = EndingTimeLimit;
}
