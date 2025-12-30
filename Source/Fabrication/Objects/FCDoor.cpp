#include "Objects/FCDoor.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Fabrication.h"

AFCDoor::AFCDoor()
	: DoorTimeline(nullptr)
	, DoorCurve(nullptr)
	, TargetYaw(110.f)
	, bIsOpen(false)
	, InitialRotation()
	, DoorFrame(nullptr)
	, InteractSpot(nullptr)
{
	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(SceneComp);

	InteractSpot = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractSpot"));
	InteractSpot->SetupAttachment(SceneComp);
	InteractSpot->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractSpot->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractSpot->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);
}

void AFCDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsOpen);

}

void AFCDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialRotation = StaticMeshComp->GetRelativeRotation();

	if (IsValid(DoorCurve) && IsValid(DoorTimeline))
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleDoorProgress"));

		DoorTimeline->AddInterpFloat(DoorCurve, ProgressFunction);
		DoorTimeline->SetLooping(false);
	}
}

void AFCDoor::Interact(ACharacter* User, const FHitResult& HitResult)
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
	if (!IsValid(Player)) return;

	Player->ServerRPCInteract(this, User, HitResult);
}

void AFCDoor::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	if (bIsOpen)
	{
		bIsOpen = false;
	}
	else
	{
		bIsOpen = true;
	}

	OnRep_IsOpen();
}

void AFCDoor::OnRep_IsOpen()
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

void AFCDoor::HandleDoorProgress(float Value)
{
	float NewYaw = FMath::Lerp(0.0f, TargetYaw, Value);

	FRotator NewRotation = InitialRotation;
	NewRotation.Yaw += NewYaw;

	StaticMeshComp->SetRelativeRotation(NewRotation);
}

