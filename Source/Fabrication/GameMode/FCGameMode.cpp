#include "GameMode/FCGameMode.h"

#include "Controller/FCPlayerController.h"
#include "GameState/FCGameState.h"
#include "PlayerState/FCPlayerState.h"

AFCGameMode::AFCGameMode()
	: WaitingTime(10.0),
	RemainTimeForPlaying(10.0),
	bReadyForPlay(false),
	MinimumPlayerCountForPlaying(2),
	bAllPlayersReady(false)
{
}

void AFCGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(
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
		UE_LOG(LogTemp, Warning, TEXT("Waiting"));
		
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
		}
		break;
	case EMatchState::Playing:
		break;
	case EMatchState::Ending:
		break;
	case EMatchState::End:
		break;
	default:
		break;
	}
}
