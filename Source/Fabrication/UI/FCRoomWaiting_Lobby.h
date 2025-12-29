#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCRoomWaiting_Lobby.generated.h"

class UTextBlock;
class UButton;
class UImage;

DECLARE_DELEGATE(FOnReadyButtonClicked);

UCLASS()
class FABRICATION_API UFCRoomWaiting_Lobby : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRoomTitle(const FText& InRoomName);
	void UpdatePlayerList(const TArray<FText>& PlayerNames, const TArray<bool>& PlayerReadyStates);
	void UpdateReadyButtonText(bool bIsReady);

	FOnReadyButtonClicked OnReadyButtonClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> ClickedSound;

private:
	UFUNCTION()
	void OnReadyButtonClickedInternal();
	
	UFUNCTION()
	void OnExitButtonClickedInternal();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyButtonText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Player1Name;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Player2Name;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Player3Name;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Player1ReadyImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Player2ReadyImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Player3ReadyImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Player1WaitImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Player2WaitImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Player3WaitImage;

};
