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
	
	FTimerHandle MainTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime;
	
	int32 RemainTimeForPlaying;
	
	uint8 bReadyForPlay : 1;

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> AlivePlayerControllers;
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> DeadPlayerControllers;

	int32 MinimumPlayerCountForPlaying;
	
	uint8 bAllPlayersReady : 1;
	
private:
	UFUNCTION()
	void OnMainTimerElapsed();

};
