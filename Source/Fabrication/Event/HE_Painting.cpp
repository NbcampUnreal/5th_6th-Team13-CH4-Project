// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/HE_Painting.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FCPlayerCharacter.h"

AHE_Painting::AHE_Painting()
{
	PrimaryActorTick.bCanEverTick = true;

	SetHazardType(EHazardType::Painting);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetIntensity(3000.0f);
	SpotLight->SetLightColor(FLinearColor::Green);

	PaintingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaintingMesh"));
	PaintingMesh->SetupAttachment(RootComponent);

	LightTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LightTriggerBox"));
	LightTriggerBox->SetupAttachment(SpotLight);
	
	LightTriggerBox->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	LightTriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	LightTriggerBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	LightTriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	bIsCool = true;
}

void AHE_Painting::BeginPlay()
{
	Super::BeginPlay();

	//if (HasAuthority())
	//{
	//	LightTrigger->OnComponentBeginOverlap.AddDynamic(this, &AHE_Painting::OnLightOverlapBegin);
	//}

	if (PaintingMesh)
	{
		PaintingMID = PaintingMesh->CreateDynamicMaterialInstance(0);
	}
}

void AHE_Painting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && bIsWatching)
	{
		TArray<AActor*> OverlappingActors;
		LightTriggerBox->GetOverlappingActors(OverlappingActors, AFCPlayerCharacter::StaticClass());

		for (AActor* Actor : OverlappingActors)
		{
			AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
			if (Player)
			{
				if (Player->GetVelocity().Size() > 10.0f)
				{
					ApplyHazardDamageWithCooldown(Player);

					UE_LOG(LogTemp, Warning, TEXT("Player Moved! Applying Damage!"));
				}
			}
		}
	}
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

void AHE_Painting::OnHazardRowReady()
{
	GetWorld()->GetTimerManager().SetTimer(
		WatchingTimerHandle,
		this,
		&AHE_Painting::ToggleWatching,
		Row->LoopInterval,
		true
	);
}

void AHE_Painting::SetWatching(bool bIsWatch)
{
	if (!PaintingMID) return;

	PaintingMID->SetTextureParameterValue(
		TEXT("PortraitTexture"),
		bIsWatch ? EyeOpenTexture : EyeClosedTexture
	);

	SpotLight->SetLightColor(bIsWatch ? FLinearColor::Red : FLinearColor::Green);

	if (bIsWatch)
	{
		LightTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		LightTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}