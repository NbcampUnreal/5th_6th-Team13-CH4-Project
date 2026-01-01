#include "Event/HE_Garden.h"
#include "Components/BoxComponent.h"
#include "Player/FCPlayerCharacter.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

AHE_Garden::AHE_Garden()
{
	PrimaryActorTick.bCanEverTick = false;

	SetHazardType(EHazardType::Garden);

	bReplicates = true;
	SetReplicateMovement(true);

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Scene);
	TriggerBox->SetBoxExtent(FVector(200.f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetGenerateOverlapEvents(true);

	bColorChanged = false;
	bDamageActive = false;
	bIsRed = false;
}

void AHE_Garden::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AHE_Garden::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AHE_Garden::OnOverlapEnd);
}

void AHE_Garden::OnHazardRowReady()
{
	check(Row);

	GetWorld()->GetTimerManager().SetTimer(
		CycleTimer,
		this,
		&AHE_Garden::StartEvent,
		Row->LoopInterval,
		true
	);
}

void AHE_Garden::StartEvent()
{
	if (!HasAuthority()) return;

	EndEvent();

	if (!FMath::RandBool())
		return;

	bIsRed = FMath::RandBool();
	bColorChanged = true;

	TArray<AActor*> ColorActors;
	TriggerBox->GetOverlappingActors(ColorActors, TargetActor);

	for (AActor* Actor : ColorActors)
	{
		Multicast_ApplyColor(Actor, bIsRed);
	}

	if (bIsRed)
	{
		const float Now = GetWorld()->GetTimeSeconds();

		for (AFCPlayerCharacter* Player : OverlappingPlayers)
		{
			PlayerRedExposureTime.Add(Player, Now);
		}

		ActivateDamage();
	}
}

void AHE_Garden::ActivateDamage()
{
	bDamageActive = true;

	GetWorld()->GetTimerManager().SetTimer(
		DamageTickTimer,
		this,
		&AHE_Garden::ApplyDamageToOverlappingActors,
		0.2f,
		true
	);
}

void AHE_Garden::ApplyDamageToOverlappingActors()
{
	if (!bDamageActive || !bIsRed) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	for (AFCPlayerCharacter* Player : OverlappingPlayers)
	{
		if (!IsValid(Player)) continue;

		float* ExposureTime = PlayerRedExposureTime.Find(Player);
		if (!ExposureTime) continue;

		if (CurrentTime - *ExposureTime >= 5.f)
		{
			ApplyHazardDamageWithCooldown(Player);
			*ExposureTime = CurrentTime;
		}
	}
}

void AHE_Garden::EndEvent()
{
	bColorChanged = false;
	bDamageActive = false;
	bIsRed = false;

	PlayerRedExposureTime.Empty();

	GetWorld()->GetTimerManager().ClearTimer(DamageTickTimer);
}

void AHE_Garden::OnTriggerBeginOverlap(
	UPrimitiveComponent*,
	AActor* OtherActor,
	UPrimitiveComponent*,
	int32,
	bool,
	const FHitResult&
)
{
	if (!HasAuthority()) return;

	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor))
	{
		OverlappingPlayers.AddUnique(Player);

		if (bDamageActive && bIsRed)
		{
			PlayerRedExposureTime.FindOrAdd(Player) = GetWorld()->GetTimeSeconds();
		}
	}
}

void AHE_Garden::OnOverlapEnd(
	UPrimitiveComponent*,
	AActor* OtherActor,
	UPrimitiveComponent*,
	int32
)
{
	if (!HasAuthority()) return;

	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor))
	{
		OverlappingPlayers.Remove(Player);
		PlayerRedExposureTime.Remove(Player);
	}
}

void AHE_Garden::Multicast_ApplyColor_Implementation(AActor* Actor, bool bRed)
{
	if (!Actor) return;

	UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>();
	if (!Mesh) return;

	UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!MID) return;

	MID->SetVectorParameterValue(
		TEXT("Color"),
		bRed ? FVector(1.f, 0.f, 0.f) : FVector(0.f, 1.f, 0.f)
	);
}
