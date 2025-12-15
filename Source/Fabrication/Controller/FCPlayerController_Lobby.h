#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController_Lobby.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class FABRICATION_API AFCPlayerController_Lobby : public APlayerController
{
	GENERATED_BODY()
	
public:
	AFCPlayerController_Lobby();
	UFUNCTION()
	void ActivateChatBox();
	UFUNCTION(Server, Reliable)
	void ServerRPCSetPlayerNickName(const FString& InNickName);
	UFUNCTION(Server, Reliable)
	void ServerRPCSendChatMessage(const FString& Message);
	UFUNCTION(Client, Reliable)
	void ClientRPCAddChatMessage(const FString& Message);
	void OnNickNameUpdated();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void ChatButtonPressed(const FInputActionValue& Value);

private:
	void UpdateNickNameUI(const FString& InNickName);
	void AddChatMessage(const FString& Message);

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> LobbyInputMapping;
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> IA_OpenChat;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> ChatWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> ChatWidgetInstance;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> ChatMessageClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> ChatMessageInstance;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> RoomListWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> RoomListWidgetInstance;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> RoomWaitingWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> RoomWaitingWidgetInstance;

};
