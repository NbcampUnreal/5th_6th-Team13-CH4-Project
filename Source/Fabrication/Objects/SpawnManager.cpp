#include "Objects/SpawnManager.h"
#include "Objects/SpawnZone.h"
#include "Data/ItemSpawnData.h"

USpawnManager::USpawnManager()
	: SpawnZones()
	, ItemList(nullptr)
	, CachedWeightPool()
	, CachedWeight(0.0f)
{
}

int32 USpawnManager::Initialize(const UDataTable* InTable)
{
	if (IsValid(InTable))
	{
		ItemList = InTable;
		FItemSpawnData* Key = ItemList->FindRow<FItemSpawnData>(FName(TEXT("KeyItem")), TEXT("SearchKeyItem"));
		if (Key)
		{
			return Key->GuaranteedAmount;
		}

		UE_LOG(LogTemp, Warning, TEXT("ItemSpawnData 데이터 테이블에 KeyItem 이 없거나 유효하지 않습니다."));
		return 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("ItemSpawnData 데이터 테이블이 없거나 유효하지 않습니다."));
	return 0;
}

void USpawnManager::RegisterSpawnZone(ASpawnZone* InSpawnZone)
{
	if (!IsValid(InSpawnZone)) return;
	
	SpawnZones.AddUnique(InSpawnZone);

}

void USpawnManager::SpawnAllItems()
{
	ShuffleSpawnZones();
	PrepareSpawn();
	ExecuteSpawn();
}

void USpawnManager::ExecuteSpawn(TSubclassOf<APickupItemBase> ItemClass, int32 Quantity)
{
	if (SpawnZones.Num() == 0 || !IsValid(ItemClass)) return;

	for (int32 i = 0; i < Quantity; ++i)
	{
		int32 SafeIndex = i % SpawnZones.Num();
		SpawnZones[SafeIndex]->SpawnActorInZone(ItemClass);
	}
}

void USpawnManager::ExecuteSpawn()
{
	if (CachedWeight <= 0.0f || SpawnZones.Num() == 0) return;

	for (ASpawnZone* Zone : SpawnZones)
	{
		float Roll = FMath::FRandRange(0.0f, CachedWeight);
		float Accumulated = 0.0f;

		for (const FWeightEntry& Entry : CachedWeightPool)
		{
			Accumulated += Entry.Weight;
			if (Roll <= Accumulated)
			{
				for (int32 i = 0; i < Entry.Quantity; ++i)
				{
					Zone->SpawnActorInZone(Entry.ItemClass);
				}
				break;
			}
		}
	}
}

void USpawnManager::PrepareSpawn()
{
	CachedWeightPool.Empty();
	CachedWeight = 0.0f;

	if (!IsValid(ItemList)) return;

	TArray<FItemSpawnData*> AllRows;
	ItemList->GetAllRows<FItemSpawnData>(TEXT("SpawnContext"), AllRows);

	for (FItemSpawnData* Row : AllRows)
	{
		if (!Row) continue;

		if (Row->SpawnType == ESpawnType::Guaranteed)
		{
			ExecuteSpawn(Row->Item, Row->GuaranteedAmount);
		}
		else
		{
			for (const TPair<int32, float>& Elem : Row->QuantityWeights)
			{
				if (Elem.Value > 0.0f)
				{
					CachedWeightPool.Add({ Row->Item, Elem.Key, Elem.Value });
					CachedWeight += Elem.Value;
				}
			}
		}
	}

}

void USpawnManager::ShuffleSpawnZones()
{
	if (SpawnZones.IsEmpty()) return;

	UE_LOG(LogTemp, Log, TEXT("Total Registered Zones: %d"), SpawnZones.Num());
	if (SpawnZones.Num() > 1)
	{
		const int32 NumZones = SpawnZones.Num();
		for (int32 i = 0; i < NumZones - 1; ++i)
		{
			int32 Index = FMath::RandRange(i, NumZones - 1);
			SpawnZones.Swap(i, Index);
		}
	}
}
