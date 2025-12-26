#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpawnManager.generated.h"

class ASpawnZone;
class APickupItemBase;

USTRUCT()
struct FWeightEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<APickupItemBase> ItemClass;
	int32 Quantity;
	float Weight;
};

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
	void ExecuteSpawn(TSubclassOf<APickupItemBase> ItemClass, int32 Quantity);
	void ExecuteSpawn();
	void PrepareSpawn();
	void ShuffleSpawnZones();

	UPROPERTY()
	TArray<TObjectPtr<ASpawnZone>> SpawnZones;
	const UDataTable* ItemList;
	UPROPERTY()
	TArray<FWeightEntry> CachedWeightPool;
	float CachedWeight;
};
