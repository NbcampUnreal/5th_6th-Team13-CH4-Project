#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController_Lobby.generated.h"

UCLASS()
class FABRICATION_API AFCPlayerController_Lobby : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, Reliable)
	void ServerRPCSetPlayerNickName(const FString& InNickName);
	UFUNCTION(Server, Reliable)
	void ServerRPCSendChatMessage(const FString& Message);
	UFUNCTION(Client, Reliable)
	void ClientRPCAddChatMessage(const FString& Message);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> ChatWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> ChatWidgetInstance;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> RoomListWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> RoomListWidgetInstance;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> RoomWaitingWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> RoomWaitingWidgetInstance;

};
