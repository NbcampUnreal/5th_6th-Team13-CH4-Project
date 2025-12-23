#include "Objects/WardrobeHideSpot.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Fabrication.h"

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
	Door->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Door->SetCollisionResponseToAllChannels(ECR_Ignore);
	Door->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);

	HideSpot = CreateDefaultSubobject<UBoxComponent>(TEXT("HideSpot"));
	HideSpot->SetupAttachment(SceneComp);
	HideSpot->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HideSpot->SetCollisionResponseToAllChannels(ECR_Ignore);
	HideSpot->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);

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
	if (HitResult.GetComponent() == Door)
	{
		AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
		if (!IsValid(Player)) return;

		Player->ServerRPCInteract(this, User, HitResult);
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Not Door Comp"));
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
