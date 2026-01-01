// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Painting.generated.h"

class UBoxComponent;
class USpotLightComponent;
class UPointLightComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Painting : public ABaseHazardEvent
{
	GENERATED_BODY()
public:
	AHE_Painting();

	virtual void BeginPlay() override;
	virtual void OnHazardRowReady() override;

protected:
	/* ===== Watching State ===== */
	UPROPERTY(ReplicatedUsing = OnRep_Watching)
	bool bIsWatching = false;

	UFUNCTION()
	void OnRep_Watching();

	void ToggleWatching();
	void SetWatching(bool bIsWatch);

	/* ===== Timer Check ===== */
	void CheckPlayersInArea();

	FTimerHandle WatchingTimerHandle;
	FTimerHandle MovementCheckTimer;

	/* ===== Components ===== */
	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* SpotLight;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PaintingMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* LightTriggerBox;

	/* ===== Material ===== */
	UPROPERTY()
	UMaterialInstanceDynamic* PaintingMID;

	UPROPERTY(EditAnywhere, Category = "Painting|Texture")
	UTexture2D* EyeOpenTexture;

	UPROPERTY(EditAnywhere, Category = "Painting|Texture")
	UTexture2D* EyeClosedTexture;

private:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
};
