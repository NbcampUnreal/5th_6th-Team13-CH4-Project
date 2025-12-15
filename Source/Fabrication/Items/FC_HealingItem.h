#pragma once

#include "CoreMinimal.h"
#include "Items/FC_BaseItem.h"
#include "FC_HealingItem.generated.h"

UCLASS()
class FABRICATION_API AFC_HealingItem : public AFC_BaseItem
{
	GENERATED_BODY()

public:
	virtual void ActivateItem(AActor* Activator) override;
};
