#include "GameMode/FCGameMode.h"

#include "Controller/FCPlayerController.h"
#include "Dialogs/SDeleteAssetsDialog.h"
#include "GameState/FCGameState.h"
#include "Event/LevelEventManager.h"
#include "PlayerState/FCPlayerState.h"
#include "Player/FCPlayerCharacter.h"//Add Item 테스트 용 
#include "Items/Inventory/FC_InventoryComponent.h"//Add Item 테스트 용 
#include "Objects/SpawnManager.h"

AFCGameMode::AFCGameMode()
	:	
	MinimumPlayerCountForPlaying(2),
	WaitingTime(5),
	RemainTimeForPlaying(WaitingTime),
	GameTimeLimit(5),
	bReadyForPlay(false),
	bAllPlayersReady(false),
	EndingTimeLimit(5),
	RemainEndingTime(EndingTimeLimit),
	GameRequireKey(0)
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

	SpawnManager = GetSpawnManger();

	AFCGameState* GS = GetGameState<AFCGameState>();
	if (IsValid(GS))
	{
		GS->SetRequiredKey(GameRequireKey);
		GS->InitializeNote();
	}

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle, 
		this, 
		&AFCGameMode::StartSpawn, 
		0.1f, 
		false);
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
		AlivePlayerControllers.AddUnique(FCPC);
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
		DeadPlayerControllers.Remove(FCPC);
	}
}

void AFCGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	Super::EndPlay(EndPlayReason);
}

void AFCGameMode::PlayerDead(APlayerController* DeadPlayer)
{
	AFCPlayerController* DeadPC = Cast<AFCPlayerController>(DeadPlayer);
	if (!IsValid(DeadPC)) return;

 	const int32 DeadPlayerIndex = AlivePlayerControllers.Find(DeadPC);
	if (DeadPlayerIndex == INDEX_NONE) return;
	
	DeadPlayerControllers.AddUnique(AlivePlayerControllers[DeadPlayerIndex]);
	AlivePlayerControllers.RemoveAt(DeadPlayerIndex);
}

void AFCGameMode::PlayerAlive(APlayerController* DeadPlayer)
{
	AFCPlayerController* PC = Cast<AFCPlayerController>(DeadPlayer);
	if (!IsValid(PC)) return;

	const int32 DeadPlayerIndex = DeadPlayerControllers.Find(PC);
	if (DeadPlayerIndex == INDEX_NONE) return;
	
	AlivePlayerControllers.AddUnique(DeadPlayerControllers[DeadPlayerIndex]);
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
			FCGS->SetRemainGameTime(GameTimeLimit);
			
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				AFCPlayerController* FCPC = Cast<AFCPlayerController>(*It);
				
				if (IsValid(FCPC))
				{
					FCPC->ClientRPCShowTimerWidget();
				}
			}
		}
		break;
		
	case EMatchState::Playing:
	{
		int32 CurrentRemainTime = FCGS->GetRemainGameTime();
		--CurrentRemainTime;
		FCGS->SetRemainGameTime(CurrentRemainTime);
		UE_LOG(LogTemp, Warning, TEXT("Playing 남은 시간 : %d"), CurrentRemainTime);
		
		if (AlivePlayerControllers.Num() <= 0 || CurrentRemainTime <= 0)
		{
			FCGS->MatchState = EMatchState::Ending;
			
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				AFCPlayerController* FCPC = Cast<AFCPlayerController>(*It);
				
				if (IsValid(FCPC))
				{
					FCPC->ClientRPCRemoveTimerWidget();
				}
			}
		}
		break;
	}
		
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
	
	AFCGameState* FCGS = GetGameState<AFCGameState>();
	if (IsValid(FCGS))
	{
		FCGS->SetRemainGameTime(GameTimeLimit);
	}
	
	RemainEndingTime = EndingTimeLimit;
}

USpawnManager* AFCGameMode::GetSpawnManger()
{
	if (!HasAuthority()) return nullptr;

	if (!IsValid(SpawnManager))
	{
		SpawnManager = NewObject<USpawnManager>(this);
		if (IsValid(ItemSpawnData))
		{
			GameRequireKey = SpawnManager->Initialize(ItemSpawnData);
		}
	}

	return SpawnManager;
}

void AFCGameMode::StartSpawn()
{
	SpawnManager->SpawnAllItems();
}
