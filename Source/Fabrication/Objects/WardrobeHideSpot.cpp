#include "Objects/WardrobeHideSpot.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BillboardComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Fabrication.h"

AWardrobeHideSpot::AWardrobeHideSpot()
	: Door(nullptr)
	, InteractSpot(nullptr)
	, DoorTimeline(nullptr)
	, DoorCurve(nullptr)
	, TargetYaw(-120.f)
	, bIsOpen(false)
	, InitialRotation()
{
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(SceneComp);
	Door->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Door->SetCollisionResponseToAllChannels(ECR_Block);
	Door->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Door->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Door->SetCollisionResponseToChannel(ECC_PickUp, ECR_Ignore);

	InteractSpot = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractSpot"));
	InteractSpot->SetupAttachment(SceneComp);
	InteractSpot->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractSpot->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractSpot->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);

	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));
}

void AWardrobeHideSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsOpen);
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
	if (HitResult.GetComponent() == InteractSpot)
	{
		AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
		if (!IsValid(Player)) return;

		Player->ServerRPCInteract(this, User, HitResult);
		
	}
}

void AWardrobeHideSpot::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	if (bIsOpen)
	{
		CloseDoor();
	}
	else
	{
		OpenDoor();
	}

	OnRep_IsOpen();
}

void AWardrobeHideSpot::OnRep_IsOpen()
{
	if (!IsValid(DoorTimeline)) return;

	if (bIsOpen)
	{
		DoorTimeline->PlayFromStart();
	}
	else
	{
		DoorTimeline->Reverse();
	}
}

void AWardrobeHideSpot::OpenDoor()
{
	if (!bIsOpen)
	{
		bIsOpen = true;
	}
}

void AWardrobeHideSpot::CloseDoor()
{
	if (bIsOpen)
	{
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
