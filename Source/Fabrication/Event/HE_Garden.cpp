#include "Event/HE_Garden.h"
#include "Components/BoxComponent.h"
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

	TriggerBox->UpdateOverlaps();

	TArray<AActor*> InitialActors;
	TriggerBox->GetOverlappingActors(InitialActors, TargetActor);

	for (AActor* Actor : InitialActors)
	{
		OverlappingActors.AddUnique(Actor);
	}

	Row = GetMyHazardRow();

	GetWorld()->GetTimerManager().SetTimer(
		CycleTimer,
		this,
		&AHE_Garden::StartEvent,
		10.0f,
		true
	);
}

void AHE_Garden::StartEvent()
{
	if (!HasAuthority()) return;

	bDamageActive = false;
	GetWorld()->GetTimerManager().ClearTimer(DamageDelayTimer);
	GetWorld()->GetTimerManager().ClearTimer(DamageTickTimer);

	if (!FMath::RandBool())
	{
		return;
	}

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
		GetWorld()->GetTimerManager().SetTimer(
			DamageDelayTimer,
			this,
			&AHE_Garden::ActivateDamage,
			5.f,
			false
		);
	}
}

void AHE_Garden::ActivateDamage()
{
	UE_LOG(LogTemp, Warning, TEXT("[Garden] Damage Activated"));
	bDamageActive = true;
	TriggerBox->UpdateOverlaps();

	GetWorld()->GetTimerManager().SetTimer(
		DamageTickTimer,
		this,
		&AHE_Garden::ApplyDamageToOverlappingActors,
		5.f,
		true
	);
}

void AHE_Garden::ApplyDamageToOverlappingActors()
{
	if (!bDamageActive || !bIsRed)
		return;

	for (AFCPlayerCharacter* Player : OverlappingPlayers)
	{
		if (IsValid(Player))
		{
			ApplyHazardDamageWithCooldown(Player);
		}
	}
}

void AHE_Garden::EndEvent()
{
	bColorChanged = false;
	bDamageActive = false;
	bIsRed = false;

	GetWorld()->GetTimerManager().ClearTimer(DamageDelayTimer);
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

		if (bDamageActive)
		{
			ApplyHazardDamageWithCooldown(Player);
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
	}
}

void AHE_Garden::Multicast_ApplyColor_Implementation(AActor* Actor, bool bRed)
{
	if (!Actor) return;

	UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>();
	if (!Mesh) return;

	UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!MID) return;

	const FVector Color = bRed
		? FVector(1.f, 0.f, 0.f)
		: FVector(0.f, 1.f, 0.f);

	MID->SetVectorParameterValue(TEXT("Color"), Color);
}

