// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FC_HazardDataRow.generated.h"

UENUM(BlueprintType)
enum class EHazardType : uint8
{
    None,
    Garden,
    Kitchen,
    Parior,
    Painting,
    Dining,
    Door,
    Bell
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FC_HazardDataRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EHazardType HazardType = EHazardType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> HazardActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LoopInterval = 0.0f;
    // Damage over time
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageAmount= 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageDelay = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;
};
