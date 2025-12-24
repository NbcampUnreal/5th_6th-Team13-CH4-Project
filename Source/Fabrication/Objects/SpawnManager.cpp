#include "Objects/SpawnManager.h"
#include "Items/Data/ItemData.h"

void USpawnManager::Initialize(const TArray<ASpawnZone*>& WorldSpawnZones, const UDataTable* InTable)
{
	SpawnZones = WorldSpawnZones;
	ItemList = InTable;
}