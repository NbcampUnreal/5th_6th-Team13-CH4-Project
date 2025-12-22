#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/Interface/Interactable.h"
#include "PickupItemBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;

UCLASS()
class FABRICATION_API APickupItemBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	APickupItemBase();
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;
	FName GetItemID() const;
	virtual void SetVisibilityPickupItem(bool bSetHidden);
	

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	
	
protected:
	UPROPERTY(VisibleDefaultsOnly)
	FName ItemID;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComp;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> InteractableWidget;

private:
	UPROPERTY(Replicated)
	uint8 bIsCollected : 1;

};
