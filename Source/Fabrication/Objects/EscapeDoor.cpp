#include "Objects/EscapeDoor.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "GameState/FCGameState.h"
#include "UI/InteractWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

AEscapeDoor::AEscapeDoor()
	: DoorTimeline(nullptr)
	, DoorCurve(nullptr)
	, TargetYaw(120.f)
	, bIsOpen(false)
	, InitialRotation()
	, LockDoorImage(nullptr)
	, UnLockDoorImage(nullptr)
	, OpenSound(nullptr)
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
	
	AFCGameState* GS = GetWorld()->GetGameState<AFCGameState>();
	if (IsValid(GS))
	{
		GS->OnCanEscape.AddUObject(this, &AEscapeDoor::UpdateWidget);
	}

	UpdateWidget(false);

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
		bIsOpen = false;
		//CloseDoor();
	}
	else
	{
		bIsOpen = true;
		//OpenDoor();
	}

	OnRep_IsOpen();
}

void AEscapeDoor::OnRep_IsOpen()
{
	if (!IsValid(DoorTimeline)) return;

	if (bIsOpen)
	{
		DoorTimeline->PlayFromStart();
		if (IsValid(OpenSound) && !HasAuthority())
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}
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

void AEscapeDoor::UpdateWidget(bool bCanEscape)
{
	if (bCanEscape)
	{
		if (UUserWidget* Widget = InteractableWidget->GetWidget())
		{
			if (UInteractWidget* Image = Cast<UInteractWidget>(Widget))
			{
				if (!ensureMsgf(UnLockDoorImage, TEXT("UnLockDoorImage 가 유효하지 않습니다. [%s]"), *GetName())) return;
				Image->SetImage(UnLockDoorImage);
				return;
			}
		}
	}
	else
	{
		if (UUserWidget* Widget = InteractableWidget->GetWidget())
		{
			if (UInteractWidget* Image = Cast<UInteractWidget>(Widget))
			{
				if (!ensureMsgf(LockDoorImage, TEXT("LockDoorImage 가 유효하지 않습니다. [%s]"), *GetName())) return;
				Image->SetImage(LockDoorImage);
				return;
			}
		}
	}
	
}

void AEscapeDoor::HandleDoorProgress(float Value)
{
	float NewYaw = FMath::Lerp(0.0f, TargetYaw, Value);

	FRotator NewRotation = InitialRotation;
	NewRotation.Yaw += NewYaw;

	StaticMeshComp->SetRelativeRotation(NewRotation);
}

