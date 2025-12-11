// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseHazardEvent.generated.h"

UCLASS(Abstract)
class FABRICATION_API ABaseHazardEvent : public AActor
{
    GENERATED_BODY()

public:
    //virtual void StartEvent() = 0;
    //virtual void EndEvent() = 0;
    //virtual void ApplyEffect() = 0;
    //virtual void StopEffect() = 0;
};
