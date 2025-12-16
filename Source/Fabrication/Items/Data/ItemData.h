#pragma once

#include "Engine/DataTable.h"
#include "Items/PickupItemBase.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName ItemID; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APickupItemBase> DropActorClass; 
};