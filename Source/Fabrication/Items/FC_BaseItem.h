#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Interface/ItemInterface.h"
#include "FC_BaseItem.generated.h"

class USphereComponent;

UCLASS()
class FABRICATION_API AFC_BaseItem : public AActor, public IItemInterface 
{
	GENERATED_BODY()
	
public:	
	AFC_BaseItem();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="Components")
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="Components")
	UStaticMeshComponent* StaticMesh; 

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ItemCollisionRadius;
	
protected:
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

	virtual void ActivateItem(AActor* Activator) override;

	virtual void DestroyItem(); 

	virtual FName GetItemID() const override;
};
