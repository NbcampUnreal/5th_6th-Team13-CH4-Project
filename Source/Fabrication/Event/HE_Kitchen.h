// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Event/Widget/HE_KitchenWidget.h"
#include "HE_Kitchen.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class FABRICATION_API AHE_Kitchen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHE_Kitchen();

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	bool bCanInteract;

};
