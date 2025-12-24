#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpawnManager.generated.h"

class ASpawnZone;

UCLASS()
class FABRICATION_API USpawnManager : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(const TArray<ASpawnZone*>& WorldSpawnZones);

private:
	UPROPERTY()
	TArray<TObjectPtr<ASpawnZone>> SpawnZones;
	
};
