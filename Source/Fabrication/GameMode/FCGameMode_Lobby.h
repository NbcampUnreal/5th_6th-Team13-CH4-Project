#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FCGameMode_Lobby.generated.h"

UENUM()
enum class EMessageType : uint8
{
	Default		UMETA(DisplayName = "Default"),
	System		UMETA(DisplayName = "System"),
};

UCLASS()
class FABRICATION_API AFCGameMode_Lobby : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFCGameMode_Lobby();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void SendChatMessage(const FString& Message, EMessageType Type = EMessageType::Default);
	virtual void Logout(AController* Exiting) override;
	
	void TravelToGameMap();
	
	void CheckAndStartGameTravel();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	FString GameMapPath = TEXT("/Game/Fabrication/Maps/TestBasicMap");
	
	// 모든 플레이어 준비 완료 후 게임 맵으로 이동할 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 TravelDelayAfterAllReady;
	
	FTimerHandle TravelToGameMapTimerHandle;

private:
	UPROPERTY()
	TArray<TObjectPtr<APlayerController>> PlayerControllers;
	
	UPROPERTY(EditDefaultsOnly, Category = "Test")
	int32 MinimumPlayerNum;

#pragma region Room

public:
	int32 CreateRoom(const FText& InRoomName, int32 MaxPlayers = 3);
	bool JoinRoom(APlayerController* InPlayer, int32 InRoomID);
	void LeaveRoom(APlayerController* InPlayer);
	void RemoveRoom(int32 InRoomID);

private:
	int32 GameModeRoomID;
#pragma endregion
};
