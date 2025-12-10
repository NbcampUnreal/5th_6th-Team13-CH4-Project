#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/Interactable.h"
#include "PickupItemBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class FABRICATION_API APickupItemBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	APickupItemBase();
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

};
