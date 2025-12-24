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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnType SpawnType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APickupItemBase> Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SpawnType == ESpawnType::Guaranteed", EditConditionHides))
	int32 GuaranteedAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SpawnType == ESpawnType::Distributed", EditConditionHides, UIMin = "0.0", UIMax = "100.0"))
	float Weight;
};