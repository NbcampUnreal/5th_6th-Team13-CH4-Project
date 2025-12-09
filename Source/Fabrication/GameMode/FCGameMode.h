#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FCGameMode.generated.h"

UCLASS()
class FABRICATION_API AFCGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	virtual void PostLogin(APlayerController *NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;
protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> AlivePlayerControllers;
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> DeadPlayerControllers;
};
