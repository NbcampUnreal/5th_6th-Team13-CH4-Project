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
	
	ItemID = TEXT("FlashLight");
}

void AFlashLight::BeginPlay()
{
	Super::BeginPlay();

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner());
	//GetOnwer() = Player(Server) || = Player(Clinet) 따라 Collision 변경 
	//클라 복제 중 랜더링 과정에서 충돌 방지 
	if (Player)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void AFlashLight::AttachSettingFlashLight()
{
	// 손전등 사용 시 Collision 비활성화 
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner());

	if (!IsValid(BoxComp) || !IsValid(StaticMeshComp)) return;

	//Owner != Player 
	if (Player)
	{
		if (Player->bUseFlashLight)
		{
			StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			StaticMeshComp->SetCollisionProfileName("PickUp");
			StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		StaticMeshComp->SetCollisionProfileName("PickUp");
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AFlashLight::SetVisibilitySpotLight(bool bIsShow)
{
	SpotLight->SetVisibility(bIsShow);
}




