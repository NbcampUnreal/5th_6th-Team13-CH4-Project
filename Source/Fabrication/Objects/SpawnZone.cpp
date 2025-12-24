#include "Objects/SpawnZone.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Fabrication.h"

ASpawnZone::ASpawnZone()
	: SceneComp(nullptr)
	, SpawnArea(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(SceneComp);
}

void ASpawnZone::BeginPlay()
{
	Super::BeginPlay();

}

AActor* ASpawnZone::SpawnActorInZone(TSubclassOf<AActor> InActor)
{
	if (!IsValid(InActor)) return nullptr;

	FVector SpawnLocation = GetRandomLocation();
	if (SpawnLocation.IsNearlyZero()) return nullptr;

	FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(InActor, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedActor)
	{
		UE_LOG(LogTemp, Log, TEXT("SpawnZone: %s 스폰 성공 위치: %s"), *SpawnedActor->GetName(), *SpawnLocation.ToString());
	}

	return SpawnedActor;
}

FVector ASpawnZone::GetRandomLocation()
{
	FVector SpawnLocation = FVector::ZeroVector;
	bool bFoundValidLocation = false;
	int32 MaxAttempts = 10;

	while (!bFoundValidLocation && MaxAttempts > 0)
	{
		MaxAttempts--;

		FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(
			SpawnArea->GetComponentLocation(),
			SpawnArea->GetScaledBoxExtent()
		);

		FVector TraceStart = FVector(RandomPoint.X, RandomPoint.Y, SpawnArea->GetComponentLocation().Z + SpawnArea->GetScaledBoxExtent().Z);
		FVector TraceEnd = FVector(RandomPoint.X, RandomPoint.Y, SpawnArea->GetComponentLocation().Z - SpawnArea->GetScaledBoxExtent().Z);

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			if (HitResult.ImpactNormal.Z > 0.7f)
			{
				SpawnLocation = HitResult.Location;
				bFoundValidLocation = true;
			}
		}

		/*
		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			SpawnLocation = HitResult.Location;

			FCollisionShape Sphere = FCollisionShape::MakeSphere(50.f);
			bool bHasObstacle = GetWorld()->OverlapAnyTestByChannel(
				SpawnLocation + FVector(0, 0, 50.f),
				FQuat::Identity,
				ECC_Pawn, // 콜리전 채널
				Sphere,
				Params
			);

			if (!bHasObstacle)
			{
				bFoundValidLocation = true;
			}
		}
		*/

	}

	return bFoundValidLocation ? SpawnLocation : FVector::ZeroVector;
}

