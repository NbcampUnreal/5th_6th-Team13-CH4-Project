// Fill out your copyright notice in the Description page of Project Settings.


#include "Flash/FlashLight.h"
#include "Components/SpotLightComponent.h"

// Sets default values
AFlashLight::AFlashLight()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComp);

	FlashSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	FlashSM->SetupAttachment(SceneComp);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(FlashSM);
}


