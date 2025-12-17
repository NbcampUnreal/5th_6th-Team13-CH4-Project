// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/HE_Painting.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"

AHE_Painting::AHE_Painting()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetIntensity(3000.0f);
	SpotLight->SetLightColor(FLinearColor::Red);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(RootComponent);
	PointLight->SetIntensity(3000.0f);
	PointLight->SetLightColor(FLinearColor::Green);
}

void AHE_Painting::SetSpotLightIntensity(float NewIntensity)
{
	if (SpotLight)
	{
		SpotLight->SetIntensity(NewIntensity);
	}
}

void AHE_Painting::SetPointLightColor(FLinearColor NewColor)
{
	if (PointLight)
	{
		PointLight->SetLightColor(NewColor);
	}
}
