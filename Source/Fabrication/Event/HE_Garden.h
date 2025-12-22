// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "Event/LevelEventManager.h"
#include "HE_Garden.generated.h"

class UBoxComponent;
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

	virtual void StartEvent() override;
	virtual void EndEvent() override;

	void CheckOverlappingActors();
	UFUNCTION()
	void ApplyColorToOverlappingActors();

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
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyColor(AActor* Actor, bool bGreen);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event")
	UBoxComponent* TriggerBox;

	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TSubclassOf<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float Interval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	bool IsChanged;


private:
	FTimerHandle ColorChangeTimer;

	FTimerHandle OverlapCheckTimer;

public:
	//TEST
	UPROPERTY()
	TMap<AActor*, UMaterialInstanceDynamic*> ActorMIDs;
};
