#include "Objects/WardrobeHideSpot.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"

AWardrobeHideSpot::AWardrobeHideSpot()
	: Door(nullptr)
	, HideSpot(nullptr)
	, DoorTimeline(nullptr)
	, DoorCurve(nullptr)
	, TargetYaw(-120.f)
	, bIsOpen(false)
	, InitialRotation()
{
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(SceneComp);
	HideSpot = CreateDefaultSubobject<UBoxComponent>(TEXT("HideSpot"));
	HideSpot->SetupAttachment(SceneComp);
	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));
}

void AWardrobeHideSpot::BeginPlay()
{
	Super::BeginPlay();

	InitialRotation = Door->GetRelativeRotation();

	if (IsValid(DoorCurve) && IsValid(DoorTimeline))
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleDoorProgress"));

		DoorTimeline->AddInterpFloat(DoorCurve, ProgressFunction);
		DoorTimeline->SetLooping(false);
	}
}

void AWardrobeHideSpot::Interact(ACharacter* User, const FHitResult& HitResult)
{
}

void AWardrobeHideSpot::ToggleDoor()
{
	if (bIsOpen) CloseDoor();
	else OpenDoor();
}

void AWardrobeHideSpot::OpenDoor()
{
	if (!bIsOpen && IsValid(DoorTimeline))
	{
		DoorTimeline->PlayFromStart();
		bIsOpen = true;
	}
}

void AWardrobeHideSpot::CloseDoor()
{
	if (bIsOpen && IsValid(DoorTimeline))
	{
		DoorTimeline->Reverse();
		bIsOpen = false;
	}
}

void AWardrobeHideSpot::HandleDoorProgress(float Value)
{
	float NewYaw = FMath::Lerp(0.0f, TargetYaw, Value);

	FRotator NewRotation = InitialRotation;
	NewRotation.Yaw += NewYaw;

	Door->SetRelativeRotation(NewRotation);
}

void AWardrobeHideSpot::OnTimelineFinished()
{
}
