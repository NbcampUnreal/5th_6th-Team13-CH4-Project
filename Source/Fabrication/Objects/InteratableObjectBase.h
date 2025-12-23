#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/Interface/Interactable.h"
#include "InteratableObjectBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class UBillboardComponent;

UCLASS()
class FABRICATION_API AInteratableObjectBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AInteratableObjectBase();
	virtual void BeginPlay() override;
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;

protected:
	UFUNCTION()
	void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComp;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UBillboardComponent> TestIneractableWidget;
};
