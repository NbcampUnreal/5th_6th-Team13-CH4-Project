// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Bell.generated.h"

class USoundBase;
class UStaticMeshComponent;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Bell : public ABaseHazardEvent
{
	GENERATED_BODY()
public:
	AHE_Bell();

protected:
	virtual void BeginPlay() override;

	virtual void StartEvent() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Scene;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> BellBoundary;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dining | Setup")
	USoundBase* BellSound;

	UFUNCTION()
	void OnOverlapBeginBell(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayBellSound();
private:
	FTimerHandle BellTimer;
	FTimerHandle BellPlayerTimer;

	bool bIsReady = true;

	void CheckPlayerVelocityAfterDelay();
	void ResetBellReady();
};
