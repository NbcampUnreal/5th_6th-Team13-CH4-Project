#pragma once

#include "CoreMinimal.h"
#include "Objects/InteratableObjectBase.h"
#include "WardrobeHideSpot.generated.h"

class UBoxComponent;

UCLASS()
class FABRICATION_API AWardrobeHideSpot : public AInteratableObjectBase
{
	GENERATED_BODY()
	
public:
	AWardrobeHideSpot();
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Door;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> HideSpot;
};
