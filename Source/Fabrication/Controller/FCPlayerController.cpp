#include "Controller/FCPlayerController.h"

void AFCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{
		return;
	}
	
	FInputModeGameOnly GameOnly;
	SetInputMode(GameOnly);
}
