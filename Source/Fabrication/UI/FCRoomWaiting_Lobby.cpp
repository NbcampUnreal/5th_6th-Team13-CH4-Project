#include "UI/FCRoomWaiting_Lobby.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UFCRoomWaiting_Lobby::SetRoomTitle(const FText& InRoomName)
{
	if (!IsValid(RoomName))
	 {
		return;
	 }
	RoomName->SetText(InRoomName);
}

void UFCRoomWaiting_Lobby::UpdatePlayerList(const TArray<FText>& PlayerNames, const TArray<bool>& PlayerReadyStates)
{
	TArray<UTextBlock*> PlayerSlots = { Player1Name, Player2Name, Player3Name };
	TArray<UImage*> ReadyImages = { Player1ReadyImage, Player2ReadyImage, Player3ReadyImage };
	TArray<UImage*> WaitImages = {Player1WaitImage, Player2WaitImage, Player3WaitImage};
	
	for (int32 i = 0; i < 3; ++i)
	{
		const bool bHasPlayer = i < PlayerNames.Num() && !PlayerNames[i].IsEmpty();
		const bool bIsReady = bHasPlayer && i < PlayerReadyStates.Num() && PlayerReadyStates[i];
		
		if (IsValid(PlayerSlots[i]))
		{
			if (bHasPlayer)
			{
				PlayerSlots[i]->SetText(PlayerNames[i]);
				PlayerSlots[i]->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				PlayerSlots[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		
		if (IsValid(ReadyImages[i]) && IsValid(WaitImages[i]))
		{
			if (bHasPlayer)
			{
				ReadyImages[i]->SetVisibility(bIsReady ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
				WaitImages[i]->SetVisibility(bIsReady ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
			}
			else
			{
				ReadyImages[i]->SetVisibility(ESlateVisibility::Collapsed);
				WaitImages[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UFCRoomWaiting_Lobby::UpdateReadyButtonText(bool bIsReady)
{
	if (IsValid(ReadyButtonText))
	{
		if (bIsReady)
		{
			ReadyButtonText->SetText(FText::FromString(TEXT("준비 취소")));
		}
		else
		{
			ReadyButtonText->SetText(FText::FromString(TEXT("준비")));
		}
	}
}

void UFCRoomWaiting_Lobby::NativeConstruct()
{
	Super::NativeConstruct();
	
	// ReadyButton 클릭 이벤트 바인딩
	if (IsValid(ReadyButton))
	{
		ReadyButton->OnClicked.AddDynamic(this, &UFCRoomWaiting_Lobby::OnReadyButtonClickedInternal);
	}
	
	// ExitButton 클릭 이벤트 바인딩
	if (IsValid(ExitButton))
	{
		ExitButton->OnClicked.AddDynamic(this, &UFCRoomWaiting_Lobby::OnExitButtonClickedInternal);
	}
}

void UFCRoomWaiting_Lobby::OnReadyButtonClickedInternal()
{
	OnReadyButtonClicked.ExecuteIfBound();
	UGameplayStatics::PlaySound2D(this, ClickedSound);
}

void UFCRoomWaiting_Lobby::OnExitButtonClickedInternal()
{	
	UGameplayStatics::PlaySound2D(this, ClickedSound);
	// 타이틀 맵으로 이동
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		// 타이틀 맵 경로
		FString TitleMapPath = TEXT("/Game/Fabrication/Maps/Title");
		
		// 클라이언트가 타이틀 맵으로 이동
		PlayerController->ClientTravel(TitleMapPath, ETravelType::TRAVEL_Absolute);
	}
}