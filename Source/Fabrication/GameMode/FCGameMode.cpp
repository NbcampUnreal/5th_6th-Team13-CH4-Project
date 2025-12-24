#include "GameMode/FCGameMode.h"

#include "Controller/FCPlayerController.h"
#include "Dialogs/SDeleteAssetsDialog.h"
#include "GameState/FCGameState.h"
#include "Event/LevelEventManager.h"
#include "PlayerState/FCPlayerState.h"
#include "Player/FCPlayerCharacter.h"//Add Item 테스트 용 
#include "Items/Inventory/FC_InventoryComponent.h"//Add Item 테스트 용 

AFCGameMode::AFCGameMode()
	:	
	MinimumPlayerCountForPlaying(2),
	WaitingTime(5),
	RemainTimeForPlaying(WaitingTime),
	GameTimeLimit(5),
	RemainGameTime(GameTimeLimit),
	bReadyForPlay(false),
	bAllPlayersReady(false),
	EndingTimeLimit(5),
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
	
	ResetValues();
	
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
	//Add iTEM 테스트 용 
	if (GetNumPlayers() == 1)
	{
		if (AFCPlayerCharacter* Ch = Cast<AFCPlayerCharacter>(NewPlayer->GetPawn()))
		{
			if (Ch->InvenComp)
			{
				Ch->InvenComp->AddItem("HealingItem", 1);
				Ch->InvenComp->AddItem("HealingItem", 1);
				Ch->InvenComp->AddItem("HealingItem", 1);
				//Ch->InvenComp->AddItem("HealingItem", 1);
				Ch->InvenComp->AddItem("FlashLight", 1);
			}
		}
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

void AFCGameMode::PlayerDead(APlayerController* DeadPlayer)
{
	int32 DeadPlayerIndex = 0;
	for (int32 i = 0; i < AlivePlayerControllers.Num(); i++)
	{
		if (DeadPlayer == AlivePlayerControllers[i])
		{
			DeadPlayerIndex = i;
			break;
		}
	}
	
	DeadPlayerControllers.Add(AlivePlayerControllers[DeadPlayerIndex]);
	AlivePlayerControllers.RemoveAt(DeadPlayerIndex);
}

void AFCGameMode::PlayerAlive(APlayerController* DeadPlayer)
{
	int32 DeadPlayerIndex = 0;
	for (int32 i = 0; i < DeadPlayerControllers.Num(); i++)
	{
		if (DeadPlayer == DeadPlayerControllers[i])
		{
			DeadPlayerIndex = i;
			break;
		}
	}
	
	AlivePlayerControllers.Add(DeadPlayerControllers[DeadPlayerIndex]);
	DeadPlayerControllers.RemoveAt(DeadPlayerIndex);
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
			UE_LOG(LogTemp, Warning, TEXT("Waiting 남은 시간 : %d"), RemainTimeForPlaying);
			// 게임 시작까지 남은 시간 출력할 곳
		}
		
		if (RemainTimeForPlaying <= 0)
		{
			FCGS->MatchState = EMatchState::Playing;
		}
		break;
		
	case EMatchState::Playing:
		
		--RemainGameTime;
		UE_LOG(LogTemp, Warning, TEXT("Playing 남은 시간 : %d"), RemainGameTime);
		
		if (AlivePlayerControllers.Num() <= 0 || RemainGameTime <= 0)
		{
			FCGS->MatchState = EMatchState::Ending;
		}
		break;
		
	case EMatchState::Ending:
		
		--RemainEndingTime;
		UE_LOG(LogTemp, Warning, TEXT("Ending 남은 시간 : %d"), RemainEndingTime);
		
		if (RemainEndingTime <= 0)
		{
			GetWorld()->ServerTravel(LobbyMapPath + TEXT("?listen"));
		}
		break;
	
	case EMatchState::End:
		break;
	
	default:
		break;
	}
}

void AFCGameMode::ResetValues()
{
	// AFCGameState* FCGS = GetGameState<AFCGameState>();
	//
	// for (APlayerState* PS : FCGS->PlayerArray)
	// {
	// 	AFCPlayerState* FCPS = Cast<AFCPlayerState>(PS);
	// 	if (IsValid(FCPS))
	// 	{
	// 		FCPS->bIsReady = false;
	// 	}
	// }
	
	GetWorldTimerManager().ClearTimer(MainTimerHandle);
	
	bReadyForPlay = false;
	bAllPlayersReady = false;
	RemainTimeForPlaying = WaitingTime;
	RemainGameTime = GameTimeLimit;
	RemainEndingTime = EndingTimeLimit;
}
