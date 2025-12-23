// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "Event/Widget/HE_KitchenWidget.h"
#include "HE_Kitchen.generated.h"


class UBoxComponent;
class UStaticMeshComponent;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Kitchen : public ABaseHazardEvent, public IInteractable
{
	GENERATED_BODY()
public:
	AHE_Kitchen();

	virtual void StartEvent() override;
	virtual void EndEvent() override; 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap_Kitchen(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnEndOverlap_Kitchen(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event")
	UBoxComponent* KitchenBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* InteractionWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InteractionWidgetClass;

private:
	bool bCanInteract;

};
