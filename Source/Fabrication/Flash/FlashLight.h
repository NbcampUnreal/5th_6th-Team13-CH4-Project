// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/PickupItemBase.h"
#include "FlashLight.generated.h"

class USpotLightComponent;

UCLASS()
class FABRICATION_API AFlashLight : public APickupItemBase
{
	GENERATED_BODY()
	
#pragma region AFlashLight Override

public:
	// Sets default values for this actor's properties
	AFlashLight();

	virtual void BeginPlay() override;
#pragma endregion

#pragma region AFlashLight Basic
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpotLightComponent> SpotLight;
#pragma endregion

#pragma region Function
public:
	UFUNCTION()
	void AttachSettingFlashLight();
	
	UFUNCTION()
	void SetVisibilitySpotLight(bool bIsShow);

#pragma endregion


};
