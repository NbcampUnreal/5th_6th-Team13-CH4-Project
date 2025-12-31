// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "Event/LevelEventManager.h"
#include "Player/FCPlayerCharacter.h"
#include "HE_Garden.generated.h"

class UBoxComponent;
class USceneComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Garden : public ABaseHazardEvent
{
	GENERATED_BODY()
public:
	AHE_Garden();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	UPROPERTY()
	TArray<AFCPlayerCharacter*> OverlappingPlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TSubclassOf<AActor> TargetActor;

	UPROPERTY()
	TMap<AFCPlayerCharacter*, float> PlayerRedExposureTime;

	bool bColorChanged;

	bool bDamageActive;

	bool bIsRed;

	FTimerHandle CycleTimer;
	FTimerHandle DamageDelayTimer;
	FTimerHandle DamageTickTimer;

	virtual void StartEvent() override;

	void ActivateDamage();

	void ApplyDamageToOverlappingActors();

	virtual void EndEvent() override;

	virtual void OnHazardRowReady() override;

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyColor(AActor* Actor, bool bRed);
};
