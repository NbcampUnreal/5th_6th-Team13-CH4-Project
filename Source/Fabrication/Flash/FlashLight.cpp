// Fill out your copyright notice in the Description page of Project Settings.


#include "Flash/FlashLight.h"
#include "Components/SpotLightComponent.h"
#include "Components/BoxComponent.h"
#include "Player/FCPlayerCharacter.h"

// Sets default values
AFlashLight::AFlashLight()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(StaticMeshComp);
}

void AFlashLight::BeginPlay()
{
	Super::BeginPlay();

	UsingFlashLight();
}

void AFlashLight::UsingFlashLight()
{
	// 손전등 사용 시 Collision 비활성화
	if (IsValid(BoxComp))
	{
		StaticMeshComp->SetCollisionProfileName("OverlapAll");
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

/// <summary>
/// 손전등을 버리거나, 죽어서 떨어지는 경우 다시 Collision 활성화
/// </summary>
void AFlashLight::DropFlashLightByOwer()
{
	// 손전등 사용 시 Collision 비활성화
	if (IsValid(BoxComp))
	{
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}




