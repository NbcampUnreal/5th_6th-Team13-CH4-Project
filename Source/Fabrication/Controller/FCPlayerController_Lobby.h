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
	void ClientRPCAddChatMessage(const FString& Message, EMessageType Type);
	void OnNickNameUpdated();
	void UpdatePlayerListUI();

protected:
	virtual void BeginPlay() override;

private:
	void UpdateNickNameUI(const FString& InNickName);
	void SetChatMessage(const FString& Message);
	void AddChatMessage(const FString& Message, EMessageType Type);

	UPROPERTY(EditDefaultsOnly, Category = "HUD", meta = (AllowPrivateAccess = true))
	TSubclassOf<UFCHUD_Lobby> HUD_LobbyClass;
	UPROPERTY()
	TObjectPtr<UFCHUD_Lobby> HUD_Lobby;
	
	FTimerHandle PlayerListUpdateTimerHandle;

#pragma region Ready
	
	UFUNCTION(BlueprintCallable)
	void ToggleReady();

	UFUNCTION(Server, Reliable)
	void ServerRPCSetReady(bool bReady);
	
#pragma endregion
	
#pragma region Room
public:
	// 방 생성 요청 (클라이언트 -> 서버)
	UFUNCTION(Server, Reliable)
	void ServerRPCCreateRoom(const FText& RoomName, int32 MaxPlayers);

	// 방 입장 요청 (클라이언트 -> 서버)
	UFUNCTION(Server, Reliable)
	void ServerRPCJoinRoom(int32 RoomID);

	// 방 나가기 요청 (클라이언트 -> 서버)
	UFUNCTION(Server, Reliable)
	void ServerRPCLeaveRoom();

	// 방 생성 결과 알림 (서버 -> 클라이언트)
	UFUNCTION(Client, Reliable)
	void ClientRPCOnRoomCreated(int32 RoomID, bool bSuccess, const FString& ErrorMessage = TEXT(""));

	// 방 입장 결과 알림 (서버 -> 클라이언트)
	UFUNCTION(Client, Reliable)
	void ClientRPCOnRoomJoined(int32 RoomID, bool bSuccess, const FString& ErrorMessage = TEXT(""));

	// 방 나가기 결과 알림 (서버 -> 클라이언트)
	UFUNCTION(Client, Reliable)
	void ClientRPCOnRoomLeft();

	// 방 목록 업데이트 알림 (서버 -> 클라이언트)
	UFUNCTION(Client, Reliable)
	void ClientRPCUpdateRoomList(const TArray<FRoomInfo>& RoomList);

#pragma endregion
};
