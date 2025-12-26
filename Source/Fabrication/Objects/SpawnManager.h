#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpawnManager.generated.h"

class ASpawnZone;
class APickupItemBase;

UCLASS()
class FABRICATION_API USpawnManager : public UObject
{
	GENERATED_BODY()
	
public:
	USpawnManager();
	void Initialize(const UDataTable* InTable);
	void RegisterSpawnZone(ASpawnZone* InSpawnZone);
	void SpawnAllItems();

private:
	void ExecuteSpawn(TSubclassOf<APickupItemBase> ItemClass);

	UPROPERTY()
	TArray<TObjectPtr<ASpawnZone>> SpawnZones;
	const UDataTable* ItemList;
};
