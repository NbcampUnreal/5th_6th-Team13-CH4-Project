#include "Objects/SpawnManager.h"

void USpawnManager::Initialize(const TArray<ASpawnZone*>& WorldSpawnZones)
{
	SpawnZones = WorldSpawnZones;
}