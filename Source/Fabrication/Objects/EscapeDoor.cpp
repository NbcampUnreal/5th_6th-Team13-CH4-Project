#include "Objects/EscapeDoor.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "GameState/FCGameState.h"

AEscapeDoor::AEscapeDoor()
	: DoorTimeline(nullptr)
	, DoorCurve(nullptr)
	, TargetYaw(120.f)
	, bIsOpen(false)
	, InitialRotation()
{
	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));
}

void AEscapeDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsOpen);
}

void AEscapeDoor::BeginPlay()
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

void AEscapeDoor::Interact(ACharacter* User, const FHitResult& HitResult)
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
	if (!IsValid(Player)) return;

	Player->ServerRPCInteract(this, User, HitResult);
}

void AEscapeDoor::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	AFCGameState* GS = GetWorld()->GetGameState<AFCGameState>();
	if (IsValid(GS))
	{
		if (!GS->CanEscape()) return;
	}

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

void AEscapeDoor::OnRep_IsOpen()
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

void AEscapeDoor::OpenDoor()
{
	if (!bIsOpen)
	{
		bIsOpen = true;
	}
}

void AEscapeDoor::CloseDoor()
{
	if (bIsOpen)
	{
		bIsOpen = false;
	}
}

void AEscapeDoor::HandleDoorProgress(float Value)
{
	float NewYaw = FMath::Lerp(0.0f, TargetYaw, Value);

	FRotator NewRotation = InitialRotation;
	NewRotation.Yaw += NewYaw;

	StaticMeshComp->SetRelativeRotation(NewRotation);
}

