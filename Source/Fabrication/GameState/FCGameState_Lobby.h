#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FCGameState_Lobby.generated.h"

USTRUCT()
struct FRoomInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 RoomID;
	UPROPERTY()
	FText RoomName;
	UPROPERTY()
	int32 CurrentPlayerCount;
	UPROPERTY()
	int32 MaxPlayerCount = 3;
};

UCLASS()
class FABRICATION_API AFCGameState_Lobby : public AGameStateBase
{
	GENERATED_BODY()
	
#pragma region RoomList

public:
	UFUNCTION()
	void OnRep_RoomList();

	UPROPERTY(ReplicatedUsing = OnRep_RoomList)
	TArray<FRoomInfo> RoomList;

#pragma endregion

#pragma region Room
	
public:
	// 방장인지 확인
	UFUNCTION(BlueprintCallable)
	bool IsRoomHost(APlayerController* PC) const;
    
	// 방장 설정
	UFUNCTION(BlueprintCallable)
	void SetRoomHost(APlayerController* PC);
    
	// 모든 플레이어가 준비되었는지 확인
	UFUNCTION(BlueprintCallable)
	bool IsAllPlayersReady() const;
    
	// Replicated 변수 - 방장 정보
	UPROPERTY(Replicated, BlueprintReadOnly)
	TObjectPtr<APlayerController> RoomHost;
    
	// Replicated 변수 - 모든 플레이어 준비 완료 여부
	UPROPERTY(ReplicatedUsing = OnRep_AllPlayersReady, BlueprintReadOnly)
	bool bAllPlayersReady;
    
	UFUNCTION()
	void OnRep_AllPlayersReady();
    
	// 모든 플레이어 준비 상태 체크 (서버에서 호출)
	void CheckAllPlayersReady();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma endregion
};
