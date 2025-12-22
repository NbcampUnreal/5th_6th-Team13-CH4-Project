#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCRoomWaiting_Lobby.generated.h"

class UTextBlock;
class UButton;

DECLARE_DELEGATE(FOnPlayButtonClicked);
DECLARE_DELEGATE(FOnExitButtonClicked);

UCLASS()
class FABRICATION_API UFCRoomWaiting_Lobby : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRoomTitle(const FText& InRoomName);
	void UpdatePlayerList(const FText& HostName, const TArray<FText>& GuestNames);
	void SetPlayButtonText(const FText& InText);

	FOnPlayButtonClicked OnPlayButtonClicked;
	FOnExitButtonClicked OnExitButtonClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PlayButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

};
