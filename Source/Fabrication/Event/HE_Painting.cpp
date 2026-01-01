#include "Event/HE_Painting.h"

#include "Components/SpotLightComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"

AHE_Painting::AHE_Painting()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SetHazardType(EHazardType::Painting);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetIntensity(3000.f);
	SpotLight->SetLightColor(FLinearColor::Green);

	PaintingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaintingMesh"));
	PaintingMesh->SetupAttachment(RootComponent);

	LightTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LightTriggerBox"));
	LightTriggerBox->SetupAttachment(RootComponent);
	LightTriggerBox->SetBoxExtent(FVector(250.f, 250.f, 200.f));
	LightTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LightTriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	LightTriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AHE_Painting::BeginPlay()
{
	Super::BeginPlay();

	if (PaintingMesh)
	{
		PaintingMID = PaintingMesh->CreateDynamicMaterialInstance(0);
	}

	OnRep_Watching();
}

void AHE_Painting::OnHazardRowReady()
{
	if (!HasAuthority()) return;

	GetWorld()->GetTimerManager().SetTimer(
		WatchingTimerHandle,
		this,
		&AHE_Painting::ToggleWatching,
		Row->LoopInterval,
		true
	);
}

void AHE_Painting::ToggleWatching()
{
	if (!HasAuthority()) return;

	bIsWatching = !bIsWatching;
	OnRep_Watching();

	if (bIsWatching)
	{
		GetWorld()->GetTimerManager().SetTimer(
			MovementCheckTimer,
			this,
			&AHE_Painting::CheckPlayersInArea,
			Row->DamageDelay,
			true
		);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimer);
	}
}

void AHE_Painting::OnRep_Watching()
{
	SetWatching(bIsWatching);
}

void AHE_Painting::SetWatching(bool bIsWatch)
{
	if (PaintingMID)
	{
		PaintingMID->SetTextureParameterValue(
			TEXT("PortraitTexture"),
			bIsWatch ? EyeOpenTexture : EyeClosedTexture
		);
	}

	if (SpotLight)
	{
		SpotLight->SetLightColor(
			bIsWatch ? FLinearColor::Red : FLinearColor::Green
		);
	}

	if (LightTriggerBox)
	{
		LightTriggerBox->SetCollisionEnabled(
			bIsWatch
			? ECollisionEnabled::QueryOnly
			: ECollisionEnabled::NoCollision
		);
	}
}

void AHE_Painting::CheckPlayersInArea()
{
	if (!HasAuthority() || !bIsWatching) return;

	TArray<AActor*> OverlappingActors;
	LightTriggerBox->GetOverlappingActors(
		OverlappingActors,
		AFCPlayerCharacter::StaticClass()
	);

	for (AActor* Actor : OverlappingActors)
	{
		AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
		if (!Player) continue;

		const float Speed = Player->GetVelocity().Size();

		if (Speed > 50.f)
		{
			ApplyHazardDamageWithCooldown(Player);
		}
	}
}

void AHE_Painting::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHE_Painting, bIsWatching);
}
