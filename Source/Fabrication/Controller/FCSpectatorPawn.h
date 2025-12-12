// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "FCSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class FABRICATION_API AFCSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

#pragma region AFCSpectatorPawn Override

public:
	virtual void PossessedBy(class AController* NewController) override;
#pragma endregion

	
};
