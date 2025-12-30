#pragma once

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

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
	// Only used when SpawnType is Guaranteed
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "SpawnType == ESpawnType::Guaranteed", EditConditionHides))
	int32 GuaranteedAmount = 1;
	// Only used when SpawnType is Distributed
	UPROPERTY(EditAnywhere, meta = (EditCondition = "SpawnType == ESpawnType::Distributed", EditConditionHides))
	TMap<int32, float> QuantityWeights;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override
	{
		EDataValidationResult Result = Super::IsDataValid(Context);

		if (!Item)
		{
			Context.AddError(FText::FromString(TEXT("아이템 클래스가 지정되지 않았습니다.")));
			Result = EDataValidationResult::Invalid;
		}

		if (SpawnType == ESpawnType::Guaranteed)
		{
			if (GuaranteedAmount <= 0)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("[%s]: Guaranteed 타입의 수량은 1개 이상이어야 합니다."), *Item->GetName())));
				Result = EDataValidationResult::Invalid;
			}
		}
		else if (SpawnType == ESpawnType::Distributed)
		{
			if (QuantityWeights.IsEmpty())
			{
				Context.AddWarning(FText::FromString(FString::Printf(TEXT("[%s]: Distributed 타입의 가중치 설정이 없습니다."), *Item->GetName())));
			}
		}

		return Result;
	}
#endif

};