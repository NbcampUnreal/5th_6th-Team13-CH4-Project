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
}

void AHE_Garden::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AHE_Garden::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AHE_Garden::OnOverlapEnd);

	// 0.1초 후에 범위 안 Actor 검사
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
	// 이미 타이머가 활성화되어 있으면 중복 시작 방지
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
	UE_LOG(LogTemp, Verbose, TEXT("Enter the apply"));
	if (OverlappingActors.Num() == 0)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[Garden] No overlapping actors"));
		return;
	}

	IsChanged = !IsChanged;

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor || !Actor->IsA(TargetActor)) continue;

		UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>();
		if (!Mesh)
		{
			UE_LOG(LogTemp, Error, TEXT("[Garden] %s has no Static Mesh Component!"), *GetNameSafe(Actor));
			continue;
		}

		UMaterialInstanceDynamic* MID = nullptr;
		if (!ActorMIDs.Contains(Actor))
		{
			MID = Mesh->CreateAndSetMaterialInstanceDynamic(0);
			if (!MID)
			{
				UE_LOG(LogTemp, Error, TEXT("[Garden] Failed to create MID for %s"), *GetNameSafe(Actor));
				continue;
			}
			ActorMIDs.Add(Actor, MID);
		}
		else
		{
			MID = ActorMIDs[Actor];
		}

		FVector Color = IsChanged ? FVector(0, 1, 0) : FVector(1, 1, 0);
		MID->SetVectorParameterValue(TEXT("Color"), Color);

		UE_LOG(LogTemp, Warning, TEXT("[Garden] %s color changed to %s"),
			*GetNameSafe(Actor),
			IsChanged ? TEXT("GREEN") : TEXT("RED"));
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

		// 타이머 시작
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
