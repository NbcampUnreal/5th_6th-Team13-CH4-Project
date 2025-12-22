#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FCGameMode_Lobby.generated.h"

UCLASS()
class FABRICATION_API AFCGameMode_Lobby : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFCGameMode_Lobby();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void SendChatMessage(const FString& Message);
	
	void TravelToGameMap();
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Test", meta = (AllowPrivateAccess))
	FString GameMapPath = TEXT("/Game/Fabrication/Maps/TestBasicMap");
	
	// 테스트용: 자동으로 게임 맵으로 이동할 시간 (초). 0이면 비활성화
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Test", meta = (AllowPrivateAccess))
	float AutoTravelDelay = 0.0f;
	
	FTimerHandle AutoTravelTimerHandle;

private:
	UPROPERTY()
	TArray<TObjectPtr<APlayerController>> PlayerControllers;

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
