#include "GameMode/FCGameMode.h"

#include "Controller/FCPlayerController.h"
#include "GameState/FCGameState.h"

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
