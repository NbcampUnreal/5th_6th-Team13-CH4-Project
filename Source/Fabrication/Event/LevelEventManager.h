// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/DataTable.h"
#include "LevelEventManager.generated.h"

/**
 * 
 */
UCLASS()
class FABRICATION_API ULevelEventManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	ULevelEventManager();

	void StartEventLoop();

	void TriggerRandomEvent();

	void SetEmergencyMode(bool bActive);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	UDataTable* HazardDataTable;
	
};
