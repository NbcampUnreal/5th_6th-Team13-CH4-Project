// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Garden.generated.h"

class UBoxComponent;
class USphereComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Garden : public ABaseHazardEvent
{
	GENERATED_BODY()
public:
	AHE_Garden();

	virtual void StartEvent() override;
	virtual void EndEvent() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event")
	USphereComponent* EventSphere;
};
