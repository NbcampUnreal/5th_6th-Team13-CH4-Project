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
	int32 Initialize(const UDataTable* InTable);
	void RegisterSpawnZone(ASpawnZone* InSpawnZone);
	void SpawnAllItems();

private:
	bool GenerateSpawnList(TArray<TSubclassOf<APickupItemBase>>& OutList);
	void DistributeItemsToZones(const TArray<TSubclassOf<APickupItemBase>>& InList);
	void ShuffleSpawnZones();

	UPROPERTY()
	TArray<TObjectPtr<ASpawnZone>> SpawnZones;
	const UDataTable* ItemList;

};
