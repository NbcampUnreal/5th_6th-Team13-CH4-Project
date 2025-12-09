// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/FCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Controller/FCPlayerCameraManager.h"

AFCPlayerController::AFCPlayerController() :
	MoveAction(nullptr),
	LookAction(nullptr),
	FCInputMappingContext(nullptr)
{
	// 플레이어 Pitch 조정을 위해 사용(-70~70)
	PlayerCameraManagerClass = AFCPlayerCameraManager::StaticClass();
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
