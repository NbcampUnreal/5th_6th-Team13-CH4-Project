// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/Interface/Interactable.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Kitchen.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class AFCPlayerCharacter;
class UParticleSystemComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Kitchen : public ABaseHazardEvent, public IInteractable
{
	GENERATED_BODY()
public:
    AHE_Kitchen();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> RootScene;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UBoxComponent> InteractionTrigger;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UWidgetComponent> InteractionWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UParticleSystemComponent> EffectComp;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY(ReplicatedUsing = OnRep_IsDone)
    bool bIsDone = false;

    UFUNCTION()
    void OnRep_IsDone();

public:
    virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
    virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;
};
