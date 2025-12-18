// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Parlor.generated.h"

class UBoxComponent;
class ULevelSequence;

/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Parlor : public ABaseHazardEvent
{
	GENERATED_BODY()

public:
    AHE_Parlor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere, Category = "Sequence")
    ULevelSequence* SequenceToPlay;

    AActor* LevelSequencePlayer;
    AActor* SequenceActor;


    bool bPlayerInside;
    float TimeInside;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
	
};
