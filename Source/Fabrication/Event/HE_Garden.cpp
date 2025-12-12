// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/HE_Garden.h"
#include "Components/SphereComponent.h"
#include "Event/FC_HazardDataRow.h"

AHE_Garden::AHE_Garden()
{
	PrimaryActorTick.bCanEverTick = true;
	SetHazardType(EHazardType::Garden);
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	EventSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SpawningSphere"));
	EventSphere->SetupAttachment(Scene);
}

void AHE_Garden::StartEvent()
{

}

void AHE_Garden::EndEvent()
{
}

