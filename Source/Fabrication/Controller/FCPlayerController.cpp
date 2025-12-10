// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/FCPlayerController.h"
#include "EnhancedInputSubsystems.h"

AFCPlayerController::AFCPlayerController() :
	MoveAction(nullptr),
	LookAction(nullptr),
	FCInputMappingContext(nullptr)
{

}

void AFCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{
		return;
	}
	
	FInputModeGameOnly GameOnly;
	SetInputMode(GameOnly);

	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* EnSubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				EnSubSystem->AddMappingContext(FCInputMappingContext, 0);
			}
		}
	}
}
