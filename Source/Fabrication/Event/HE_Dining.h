// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Dining.generated.h"

class UBoxComponent;
class USoundBase;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Dining : public ABaseHazardEvent
{
	GENERATED_BODY()
public:
	AHE_Dining();
protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UBoxComponent> InteractionTrigger;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> SpawnLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dining | Setup")
    TSubclassOf<AActor> ItemClassToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dining | Setup")
    USoundBase* ItemSound;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_DropItem(AActor* SpawnedItem);

private:
    bool bHasHappened = false;
    FVector GetRandomPointInVolume_Sphere() const;
};
