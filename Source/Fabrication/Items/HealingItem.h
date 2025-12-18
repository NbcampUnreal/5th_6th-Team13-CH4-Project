#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItemBase.h"
#include "HealingItem.generated.h"

UCLASS()
class FABRICATION_API AHealingItem : public APickupItemBase
{
	GENERATED_BODY()
	
#pragma region AHealingItem Override

public:
	AHealingItem();

	virtual void BeginPlay() override;
#pragma endregion

#pragma region Function
public:
	UFUNCTION()
	void AttachSettingHealingItem();
	
	UFUNCTION()
	void SetVisbilityHealItem(bool bIsShow);
#pragma endregion

};
