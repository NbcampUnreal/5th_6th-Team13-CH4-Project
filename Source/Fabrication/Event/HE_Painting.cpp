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

	PaintingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaintingMesh"));
	PaintingMesh->SetupAttachment(RootComponent);
}

void AHE_Painting::BeginPlay()
{
	Super::BeginPlay();

	if (PaintingMesh)
	{
		PaintingMID = PaintingMesh->CreateDynamicMaterialInstance(0);
	}

	GetWorld()->GetTimerManager().SetTimer(
		WatchingTimerHandle,
		this,
		&AHE_Painting::ToggleWatching,
		3.0f,
		true
	);
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

void AHE_Painting::ToggleWatching()
{
	bIsWatching = !bIsWatching;
	SetWatching(bIsWatching);
}

void AHE_Painting::SetWatching(bool bIsWatch)
{
	if (!PaintingMID) return;

	PaintingMID->SetTextureParameterValue(
		TEXT("PortraitTexture"),
		bIsWatch ? EyeOpenTexture : EyeClosedTexture
	);

	SpotLight->SetVisibility(bIsWatch);
	PointLight->SetVisibility(!bIsWatch);
}