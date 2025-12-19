// Fill out your copyright notice in the Description page of Project Settings.

#include "Event/HE_Garden.h"
#include "Components/BoxComponent.h"
#include "Event/FC_HazardDataRow.h"

AHE_Garden::AHE_Garden()
{
	PrimaryActorTick.bCanEverTick = true;

	SetHazardType(EHazardType::Garden);

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Scene);

	TriggerBox->SetBoxExtent(FVector(200.f, 200.f, 200.f));
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetCollisionProfileName("Trigger");
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetHiddenInGame(false);

	IsChanged = false;

	bReplicates = true;
	SetReplicateMovement(true);
}

void AHE_Garden::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AHE_Garden::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AHE_Garden::OnOverlapEnd);


	GetWorld()->GetTimerManager().SetTimer(
		OverlapCheckTimer,
		this,
		&AHE_Garden::CheckOverlappingActors,
		0.1f,
		true
	);
}

void AHE_Garden::CheckOverlappingActors()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			TArray<AActor*> FoundActors;
			TriggerBox->GetOverlappingActors(FoundActors, TargetActor);

			for (AActor* Actor : FoundActors)
			{
				if (Actor->IsA(TargetActor))
				{
					OverlappingActors.AddUnique(Actor);
					UE_LOG(LogTemp, Warning, TEXT("[Garden] Found Actor in range (delayed): %s"), *Actor->GetName());
				}
			}

			if (OverlappingActors.Num() > 0)
			{
				StartEvent();
			}
		});
}
void AHE_Garden::StartEvent()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ColorChangeTimer))
	{
		return;
	}

	const FC_HazardDataRow* Row = GetMyHazardRow();
	if (!Row) return;

	Interval = Row->LoopInterval;
	UE_LOG(LogTemp, Warning, TEXT("[Garden] StartEvent - Interval: %f"), Interval);

	TriggerBox->UpdateOverlaps();
	GetWorld()->GetTimerManager().SetTimer(
		ColorChangeTimer,
		this,
		&AHE_Garden::ApplyColorToOverlappingActors,
		2.0f,
		true
	);

	UE_LOG(LogTemp, Warning, TEXT("[Garden] Color Timer Started"));
}

void AHE_Garden::EndEvent()
{
	GetWorld()->GetTimerManager().ClearTimer(ColorChangeTimer);
	UE_LOG(LogTemp, Warning, TEXT("[Garden] Color Timer Stopped"));
}

void AHE_Garden::ApplyColorToOverlappingActors()
{
	if (!HasAuthority()) return;

	IsChanged = !IsChanged;

	for (AActor* Actor : OverlappingActors)
	{
		Multicast_ApplyColor(Actor, IsChanged);
	}
}

void AHE_Garden::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor && TargetActor && OtherActor->IsA(TargetActor))
	{
		OverlappingActors.AddUnique(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("[Garden] BeginOverlap: %s"), *GetNameSafe(OtherActor));

		//StartEvent();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Nothing"));
	}
}

void AHE_Garden::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (OtherActor && TargetActor && OtherActor->IsA(TargetActor))
	{
		OverlappingActors.Remove(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("[Garden] EndOverlap: %s"), *GetNameSafe(OtherActor));

		if (OverlappingActors.Num() == 0)
		{
			EndEvent();
		}
	}
}

void AHE_Garden::Multicast_ApplyColor_Implementation(AActor* Actor, bool bGreen)
{
	if (!Actor) return;

	UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>();
	if (!Mesh) return;

	UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!MID) return;

	FVector Color = bGreen ? FVector(0, 1, 0) : FVector(1, 1, 0);
	MID->SetVectorParameterValue(TEXT("Color"), Color);
}

