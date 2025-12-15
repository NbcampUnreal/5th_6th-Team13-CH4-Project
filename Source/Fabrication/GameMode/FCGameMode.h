#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FCGameMode.generated.h"

UCLASS()
class FABRICATION_API AFCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFCGameMode();
	
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController *NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> AlivePlayerControllers;
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> DeadPlayerControllers;
	
	FTimerHandle MainTimerHandle;
	
	FTimerHandle GameTimeLimitHandle;

	int32 MinimumPlayerCountForPlaying;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime;
	
	int32 RemainTimeForPlaying;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 GameTimeLimit;
	
	int32 RemainGameTime;
	
	uint8 bReadyForPlay : 1;

	uint8 bAllPlayersReady : 1;
	
private:
	UFUNCTION()
	void OnMainTimerElapsed();

	UFUNCTION()
	void DecreaseGameTime();

	void ResetValues();

};
