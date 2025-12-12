#include "GameMode/FCGameMode.h"

#include "Controller/FCPlayerController.h"
#include "GameState/FCGameState.h"
#include "Event/LevelEventManager.h"

void AFCGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (ULevelEventManager* Manager = GetWorld()->GetSubsystem<ULevelEventManager>())
	{
		Manager->HazardDataTable = SetHazardDataTable;
		Manager->StartEventLoop(EHazardType::Garden);
	}
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
