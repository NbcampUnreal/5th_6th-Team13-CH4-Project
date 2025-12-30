#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController_Lobby.generated.h"

class UFCHUD_Lobby;
enum class EMessageType : uint8;

UCLASS()
class FABRICATION_API AFCPlayerController_Lobby : public APlayerController
{
	GENERATED_BODY()
	
public:
	AFCPlayerController_Lobby();
	UFUNCTION(Server, Reliable)
	void ServerRPCSetPlayerNickName(const FString& InNickName);
	UFUNCTION(Server, Reliable)
	void ServerRPCSendChatMessage(const FString& Message);
	UFUNCTION(Client, Reliable)
	void ClientRPCAddChatMessage(const FString& Message, const EMessageType& Type);
	void OnNickNameUpdated();
	void UpdatePlayerListUI();

	UFUNCTION(Client, Reliable)
	void ClientRPCShowGameTipWidget();
	
protected:
	virtual void BeginPlay() override;

private:
	void UpdateNickNameUI(const FString& InNickName);
	void SetChatMessage(const FString& Message);
	void AddChatMessage(const FString& Message, const EMessageType& Type);
	

	UPROPERTY(EditDefaultsOnly, Category = "HUD", meta = (AllowPrivateAccess = true))
	TSubclassOf<UFCHUD_Lobby> HUD_LobbyClass;
	UPROPERTY()
	TObjectPtr<UFCHUD_Lobby> HUD_Lobby;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> GameTipWidget;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> GameTipInstance;
	
	FTimerHandle PlayerListUpdateTimerHandle;

#pragma region Ready
	
	UFUNCTION(BlueprintCallable)
	void ToggleReady();

	UFUNCTION(Server, Reliable)
	void ServerRPCSetReady(bool bReady);
	
#pragma endregion
};
