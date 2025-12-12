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
	void ServerSendPlayerNickName(const FString& InNickName);
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& Message);
	UFUNCTION(Client, Reliable)
	void ClientAddChatMessage(const FString& Message);

protected:
	virtual void BeginPlay() override;
};
