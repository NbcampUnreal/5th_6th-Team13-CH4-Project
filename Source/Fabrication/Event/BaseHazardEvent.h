// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Event/FC_HazardDataRow.h"
#include "BaseHazardEvent.generated.h"

UCLASS(Abstract)
class FABRICATION_API ABaseHazardEvent : public AActor
{
    GENERATED_BODY()

protected:
    ABaseHazardEvent() = default;

public:
    virtual void StartEvent() PURE_VIRTUAL(ABaseHazardEvent::StartEvent, );
    virtual void EndEvent() PURE_VIRTUAL(ABaseHazardEvent::EndEvent, );
    void ApplyEffect(UParticleSystem* Effect, FVector EffectLocation);
    void StopEffect();
    virtual void SetHazardType(EHazardType Type);
    virtual EHazardType GetHazardType();

protected:
    const FC_HazardDataRow* GetMyHazardRow() const;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayEffect(UParticleSystem* Effect, FVector EffectLocation);
private:
    EHazardType HazardType = EHazardType::None;

protected:
    TMap<AActor*, float> LastDamageTimeMap;
    float LastDamageTime = 0.0f;
    bool CanApplyDamage();
    UFUNCTION(BlueprintCallable, Category = "Hazard")
    void ApplyHazardDamageWithCooldown(AActor* TargetActor);
};
