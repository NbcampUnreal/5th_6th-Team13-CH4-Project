#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Event/FC_HazardDataRow.h"
#include "FCGameMode.generated.h"

UCLASS()
class FABRICATION_API AFCGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
private:
		
	virtual void PostLogin(APlayerController *NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;
protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> AlivePlayerControllers;
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> DeadPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	UDataTable* SetHazardDataTable;
};
