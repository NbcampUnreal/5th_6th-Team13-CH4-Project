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

	void SetSpotLightIntensity(float NewIntensity);

	void SetPointLightColor(FLinearColor NewColor);

private:
	UPROPERTY(VisibleAnywhere)
	class USpotLightComponent* SpotLight;

	UPROPERTY(VisibleAnywhere)
	class UPointLightComponent* PointLight;
	
};
