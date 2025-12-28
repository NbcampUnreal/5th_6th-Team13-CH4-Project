// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/HE_Painting.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/CapsuleComponent.h"
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

	LightTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LightTrigger"));
	LightTrigger->SetupAttachment(SpotLight);
	
	LightTrigger->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	LightTrigger->SetCapsuleHalfHeight(300.0f);
	LightTrigger->SetCapsuleRadius(100.0f);

	LightTrigger->SetCollisionProfileName(TEXT("Trigger"));

	LightTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	LightTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

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

	GetWorld()->GetTimerManager().SetTimer(
		WatchingTimerHandle,
		this,
		&AHE_Painting::ToggleWatching,
		3.0f,
		true
	);
}

void AHE_Painting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && bIsWatching)
	{
		TArray<AActor*> OverlappingActors;
		LightTrigger->GetOverlappingActors(OverlappingActors, AFCPlayerCharacter::StaticClass());

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

//void AHE_Painting::OnLightOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Overlap Detected with: %s"), *OtherActor->GetName());
//
//	const FC_HazardDataRow* Row = GetMyHazardRow();
//	if (!Row)
//	{
//		UE_LOG(LogTemp, Error, TEXT("Hazard Row Null!"));
//		return;
//	}
//	if (bIsWatching && OtherActor && OtherActor != this)
//	{
//		if (OtherActor->IsA(AFCPlayerCharacter::StaticClass()))
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Player hit by Light!"));
//			ApplyHazardDamageWithCooldown(OtherActor);
//		}
//	}
//}

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
		LightTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		LightTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}