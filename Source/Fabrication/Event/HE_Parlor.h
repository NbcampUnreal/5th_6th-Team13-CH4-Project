// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "Materials/MaterialInterface.h"
#include "HE_Parlor.generated.h"

class UBoxComponent;
class ULevelSequence;
class UPostProcessComponent;
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

    UPROPERTY(VisibleAnywhere)
    UPostProcessComponent* PostProcessComponent;

    UPROPERTY(EditAnywhere, Category = "PostProcess")
    UMaterialInterface* PostProcessMaterial;

    UPROPERTY(EditAnywhere, Category = "PostProcess")
    float TotalDuration = 3.f;

    UMaterialInstanceDynamic* PostProcessMID;

    bool bPlayerInside;
    float TimeInside;
    float RequiredOverlapTime;

    float Elapsed = 0.f;
    float Alpha = 0.f;          
    bool bFadingIn = false;
    bool bPostProcessActivated = false;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
