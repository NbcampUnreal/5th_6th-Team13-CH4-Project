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
	FString GameMapPath = TEXT("/Game/Fabrication/Maps/TempMap");
	
	// 모든 플레이어 준비 완료 후 게임 맵으로 이동할 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 TravelDelayAfterAllReady;
	
	FTimerHandle TravelToGameMapTimerHandle;
	FTimerHandle TravelDelayCountdownTimerHandle;

private:
	UPROPERTY()
	TArray<TObjectPtr<APlayerController>> PlayerControllers;
	
	UPROPERTY(EditDefaultsOnly, Category = "Test")
	int32 MinimumPlayerNum;
	
	// 게임 시작까지 남은 시간 (초)
	int32 RemainingTravelDelay;
	
	// 1초마다 호출되는 카운트다운 함수
	void UpdateTravelDelayCountdown();
};
