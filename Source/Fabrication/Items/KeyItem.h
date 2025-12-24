#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItemBase.h"
#include "KeyItem.generated.h"

UCLASS()
class FABRICATION_API AKeyItem : public APickupItemBase
{
	GENERATED_BODY()

public:
	AKeyItem();
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;

};
