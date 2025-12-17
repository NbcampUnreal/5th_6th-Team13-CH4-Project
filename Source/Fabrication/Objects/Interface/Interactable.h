#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class FABRICATION_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) = 0;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) = 0;
};
