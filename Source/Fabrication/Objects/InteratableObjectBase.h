#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/Interface/Interactable.h"
#include "InteratableObjectBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class FABRICATION_API AInteratableObjectBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AInteratableObjectBase();
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

};
