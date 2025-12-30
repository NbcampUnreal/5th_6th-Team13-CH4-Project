#include "Objects/SpawnManager.h"
#include "Objects/SpawnZone.h"
#include "Data/ItemSpawnData.h"

USpawnManager::USpawnManager()
	: SpawnZones()
	, ItemList(nullptr)
{
}

void USpawnManager::Initialize(const UDataTable* InTable)
{
	if (IsValid(InTable))
	{
		ItemList = InTable;
	}
}

void USpawnManager::RegisterSpawnZone(ASpawnZone* InSpawnZone)
{
	if (!IsValid(InSpawnZone)) return;
	
	SpawnZones.AddUnique(InSpawnZone);

}

void USpawnManager::SpawnAllItems()
{
	TArray<FItemSpawnData*> AllRows;
	ItemList->GetAllRows<FItemSpawnData>(TEXT("SpawnContext"), AllRows);

	TArray<FItemSpawnData*> DistributedItems;
	float TotalWeight = 0.0f;

	for (FItemSpawnData* Row : AllRows)
	{
		if (Row->SpawnType == ESpawnType::Guaranteed)
		{
			for (int32 i = 0; i < Row->GuaranteedAmount; ++i)
			{
				ExecuteSpawn(Row->Item);
			}
		}
		else if (Row->SpawnType == ESpawnType::Distributed)
		{
			DistributedItems.Add(Row);
			TotalWeight += Row->Weight;
		}
	}

	if (TotalWeight > 0.0f)
	{
		for (ASpawnZone* Zone : SpawnZones)
		{
			float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
			float CurrentWeight = 0.0f;

			for (FItemSpawnData* Row : DistributedItems)
			{
				CurrentWeight += Row->Weight;
				if (RandomValue <= CurrentWeight)
				{
					Zone->SpawnActorInZone(Row->Item);
					break;
				}
			}
		}
	}
}

void USpawnManager::ExecuteSpawn(TSubclassOf<APickupItemBase> ItemClass)
{
	if (SpawnZones.Num() == 0 || !IsValid(ItemClass)) return;

	int32 TargetIndex = FMath::RandRange(0, SpawnZones.Num() - 1);

	if (SpawnZones[TargetIndex])
	{
		SpawnZones[TargetIndex]->SpawnActorInZone(ItemClass);
	}
}