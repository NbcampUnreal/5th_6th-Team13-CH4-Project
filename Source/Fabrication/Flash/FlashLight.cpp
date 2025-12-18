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

	AttachSettingFlashLight();
}

void AFlashLight::AttachSettingFlashLight()
{
	// 손전등 사용 시 Collision 비활성화 
	if (IsValid(BoxComp))
	{
		//static mesh NoCollision -> PickUp 불가.테스트 중이라 주석 처리  
		/*StaticMeshComp->SetCollisionProfileName("OverlapAll");
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
		BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		/*SpotLight->SetVisibility(false); ServerRPCChangeOnFlashLight()에서 처리*/
	}
}

void AFlashLight::SetVisibilitySpotLight(bool bIsShow)
{
	SpotLight->SetVisibility(bIsShow);
}

void AFlashLight::SetVisbilityFlashLight(bool bIsShow)
{
	SetActorHiddenInGame(bIsShow);
}




