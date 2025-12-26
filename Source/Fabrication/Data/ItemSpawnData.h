#pragma once


#include "Engine/DataTable.h"
#include "Items/PickupItemBase.h"
#include "ItemSpawnData.generated.h"

UENUM(BlueprintType)
enum class ESpawnType : uint8
{

	Guaranteed		UMETA(DisplayName = "Guaranteed"),
	Distributed		UMETA(DisplayName = "Distributed"),

};

USTRUCT(BlueprintType)
struct FItemSpawnData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	ESpawnType SpawnType;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickupItemBase> Item;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "SpawnType == ESpawnType::Guaranteed", EditConditionHides))
	int32 GuaranteedAmount = 1;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "SpawnType == ESpawnType::Distributed", EditConditionHides))
	TMap<int32, float> QuantityWeights;

};