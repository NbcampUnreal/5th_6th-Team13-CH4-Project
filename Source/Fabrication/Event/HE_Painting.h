// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Painting.generated.h"

/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Painting : public ABaseHazardEvent
{
	GENERATED_BODY()
public :
	AHE_Painting();

	virtual void BeginPlay() override;

	void SetSpotLightIntensity(float NewIntensity);

	void SetPointLightColor(FLinearColor NewColor);
	
	void SetWatching(bool bIsWatching);

	void ToggleWatching();

	UPROPERTY(VisibleAnywhere)
	bool bIsWatching = false;

protected:
	UPROPERTY(VisibleAnywhere)
	class USpotLightComponent* SpotLight;

	UPROPERTY(VisibleAnywhere)
	class UPointLightComponent* PointLight;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PaintingMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* PaintingMID;

	UPROPERTY(EditAnywhere, Category = "Painting|Texture")
	UTexture2D* EyeOpenTexture;

	UPROPERTY(EditAnywhere, Category = "Painting|Texture")
	UTexture2D* EyeClosedTexture;

private:
	FTimerHandle WatchingTimerHandle;
};
