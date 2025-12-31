#include "Event/HE_Parlor.h"

#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FCPlayerCharacter.h"
#include "Engine/World.h"

AHE_Parlor::AHE_Parlor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetHazardType(EHazardType::Parior);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AHE_Parlor::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AHE_Parlor::OnOverlapEnd);

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(RootComponent);
	PostProcessComponent->bUnbound = true;
	PostProcessComponent->bEnabled = false;
}

void AHE_Parlor::BeginPlay()
{
	Super::BeginPlay();

	if (PostProcessMaterial)
	{
		PostProcessMID = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);
		PostProcessComponent->Settings.WeightedBlendables.Array.Add(
			FWeightedBlendable(1.f, PostProcessMID)
		);
		PostProcessMID->SetScalarParameterValue(TEXT("Intensity"), 0.f);
	}
}

void AHE_Parlor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFadingIn && PostProcessMID)
	{
		Elapsed += DeltaTime;

		float t = FMath::Clamp(Elapsed / TotalDuration, 0.f, 1.f);
		float easedT = t * t;

		Alpha = FMath::Lerp(0.f, 1.f, easedT);
		PostProcessMID->SetScalarParameterValue(TEXT("Intensity"), Alpha);

		if (t >= 1.f)
		{
			bFadingIn = false;
		}
	}
}

void AHE_Parlor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor);
	if (!Player) return;

	InsidePlayers.Add(Player);

	PostProcessComponent->bEnabled = true;
	bFadingIn = true;
	Elapsed = 0.f;

	if (!DamageStartTimerMap.Contains(Player))
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(
			Handle,
			FTimerDelegate::CreateUObject(
				this,
				&AHE_Parlor::EnableParlorDamage,
				Player
			),
			Row->LoopInterval,
			false
		);

		DamageStartTimerMap.Add(Player, Handle);
	}
}

void AHE_Parlor::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor);
	if (!Player) return;

	InsidePlayers.Remove(Player);

	if (FTimerHandle* Handle = DamageStartTimerMap.Find(Player))
	{
		GetWorld()->GetTimerManager().ClearTimer(*Handle);
		DamageStartTimerMap.Remove(Player);
	}

	bFadingIn = false;
	Elapsed = 0.f;
	Alpha = 0.f;

	if (PostProcessMID)
	{
		PostProcessMID->SetScalarParameterValue(TEXT("Intensity"), 0.f);
	}

	PostProcessComponent->bEnabled = false;
}

void AHE_Parlor::EnableParlorDamage(AFCPlayerCharacter* Player)
{
	if (!HasAuthority()) return;
	if (!IsValid(Player)) return;

	if (!InsidePlayers.Contains(Player)) return;

	UE_LOG(LogTemp, Warning, TEXT("[Parior] Damage Started"));
	ApplyHazardDamageWithCooldown(Player);
}
