#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/Interactable.h"
#include "InteratableObjectBase.generated.h"

class USceneComponent;

UCLASS()
class FABRICATION_API AInteratableObjectBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AInteratableObjectBase();
	virtual void Interact(ACharacter* User) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;

};
