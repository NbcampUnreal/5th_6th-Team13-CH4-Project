#include "Controller/FCPlayerController_Title.h"
#include "GameInstance/FCGameInstance.h"
#include "UI/FCTitleLayout.h"

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
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
			
			bShowMouseCursor = true;
			
			if (UFCTitleLayout* NameText = Cast<UFCTitleLayout>(TitleWidgetInstance))
			{
				NameText->FocusNameInput();
			}

		}
	}
}

void AFCPlayerController_Title::SetPlayerNickName(const FString& InNickName)
{
	UFCGameInstance* FCGameInstance = GetGameInstance<UFCGameInstance>();
	if (!IsValid(FCGameInstance)) return;

	FCGameInstance->SetLocalPlayerNickName(InNickName);
}

void AFCPlayerController_Title::JoinServer(const FString& InIPAddress, const FString& InNickName)
{
	UFCGameInstance* FCGI = GetGameInstance<UFCGameInstance>();
	if (!IsValid(FCGI))
	{
		return;
	}
	
	FCGI->SetLocalPlayerNickName(InNickName);
	ClientTravel(InIPAddress, TRAVEL_Absolute);
}
