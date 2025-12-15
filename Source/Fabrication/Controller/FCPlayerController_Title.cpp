#include "Controller/FCPlayerController_Title.h"

#include "Blueprint/UserWidget.h"

void AFCPlayerController_Title::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{
		return;
	}
	
	if (IsValid(TitleWidgetClass))
	{
		TitleWidgetInstance = CreateWidget<UUserWidget>(this, TitleWidgetClass);
		
		if (IsValid(TitleWidgetInstance))
		{
			TitleWidgetInstance->AddToViewport();
			
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(TitleWidgetInstance->GetCachedWidget());
			SetInputMode(InputMode);
			
			bShowMouseCursor = true;
		}
	}
}

void AFCPlayerController_Title::JoinServer(const FString& InIPAddress)
{
	ClientTravel(InIPAddress, TRAVEL_Absolute);
}
