#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Event/FC_HazardDataRow.h"
#include "FCGameMode.generated.h"

class USpawnManager;

UCLASS()
class FABRICATION_API AFCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFCGameMode();

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController *NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	FORCEINLINE TArray<APlayerController*> GetPlayerControllerArray() const { return AlivePlayerControllers; }
	FORCEINLINE TArray<APlayerController*> GetDeadPlayerControllerArray() const { return DeadPlayerControllers; }

	void PlayerDead(APlayerController* DeadPlayer);
	void PlayerAlive(APlayerController* DeadPlayer);
protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> AlivePlayerControllers;
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> DeadPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	UDataTable* SetHazardDataTable;
	
	FTimerHandle MainTimerHandle;
	
	FTimerHandle GameTimeLimitHandle;

	FTimerHandle EndingTimerHandle;
	
	int32 MinimumPlayerCountForPlaying;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime;

	int32 RemainTimeForPlaying;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 GameTimeLimit;
	
	int32 RemainGameTime;
	
	uint8 bReadyForPlay : 1;
	uint8 bAllPlayersReady : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 EndingTimeLimit;

	int32 RemainEndingTime;
	
private:
	UFUNCTION()
	void OnMainTimerElapsed();

	// UFUNCTION()
	// void DecreaseGameTime();
	//
	// UFUNCTION()
	// void DecreaseEndingTime();
	
	void ResetValues();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	FString LobbyMapPath = TEXT("/Game/Fabrication/Maps/Lobby");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	FString GameMapPath = TEXT("/Game/Fabrication/Maps/TestBasicMap");

#pragma region Spawn

public:
	USpawnManager* GetSpawnManger();

private:
	UPROPERTY()
	TObjectPtr<USpawnManager> SpawnManager;
	UPROPERTY(EditDefaultsOnly, Category = "ItemDataTable")
	TObjectPtr<UDataTable> ItemSpawnData;

#pragma endregion
};
