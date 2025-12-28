#include "Objects/SpawnManager.h"
#include "Objects/SpawnZone.h"
#include "Data/ItemSpawnData.h"

USpawnManager::USpawnManager()
	: SpawnZones()
	, ItemList(nullptr)
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
	TArray<TSubclassOf<APickupItemBase>> SpawnList;
	if (GenerateSpawnList(SpawnList))
	{
		DistributeItemsToZones(SpawnList);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GenerateSpawList(SpawnList) 값이 false 입니다."));
	}
}

bool USpawnManager::GenerateSpawnList(TArray<TSubclassOf<APickupItemBase>>& OutList)
{
	TArray<FItemSpawnData*> AllRows;
	ItemList->GetAllRows<FItemSpawnData>(TEXT("Generating Spawn List"), AllRows);

	for (FItemSpawnData* Row : AllRows)
	{
		if (!Row || !Row->Item) continue;

		switch (Row->SpawnType)
		{

		case ESpawnType::Guaranteed: 
		{
			for (int32 i = 0; i < Row->GuaranteedAmount; ++i)
			{
				OutList.Add(Row->Item);
			}
		}
			break;

		case ESpawnType::Distributed:
		{
			int32 SelectedQuantity = 0;
			float TotalWeight = 0.0f;
			
			for (const TPair<int32, float>& Pair : Row->QuantityWeights)
			{
				TotalWeight += Pair.Value;
			}

			if (TotalWeight > 0.0f)
			{
				float Roll = FMath::FRandRange(0.0f, TotalWeight);
				float Accumulated = 0.0f;

				for (const TPair<int32, float>& Pair : Row->QuantityWeights)
				{
					Accumulated += Pair.Value;
					if (Roll <= Accumulated)
					{
						SelectedQuantity = Pair.Key;
						break;
					}
				}
			}

			for (int32 i = 0; i < SelectedQuantity; ++i)
			{
				OutList.Add(Row->Item);
			}
		}
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("알 수 없는 SpawnType 입니다. : [%s]"), *Row->Item->GetName());
			return false;
		}
	}

	if (OutList.Num() > 1)
	{
		const int32 LastIndex = OutList.Num() - 1;

		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			OutList.Swap(i, Index);
		}
	}

	return !OutList.IsEmpty();
}

void USpawnManager::DistributeItemsToZones(const TArray<TSubclassOf<APickupItemBase>>& InList)
{
	for (int32 i = 0; i < InList.Num(); ++i)
	{
		int32 ZoneIndex = i % SpawnZones.Num();

		if (IsValid(SpawnZones[ZoneIndex]))
		{
			SpawnZones[ZoneIndex]->SpawnActorInZone(InList[i]);
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
