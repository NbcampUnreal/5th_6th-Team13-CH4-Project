#include "Objects/FCDoor.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Fabrication.h"
#include "Kismet/GameplayStatics.h"

AFCDoor::AFCDoor()
	: DoorTimeline(nullptr)
	, DoorCurve(nullptr)
	, TargetYaw(110.f)
	, bIsOpen(false)
	, InitialRotation()
	, DoorFrame(nullptr)
	, InteractSpot(nullptr)
	, OpenSound(nullptr)
	, CloseSound(nullptr)
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
		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName("OnTimelineFinished"));

		DoorTimeline->AddInterpFloat(DoorCurve, ProgressFunction);
		DoorTimeline->SetTimelineFinishedFunc(FinishedFunction);
		DoorTimeline->SetLooping(false);
	}

	ensureMsgf(OpenSound, TEXT("OpenSound 가 유효하지 않습니다. [%s]"), *GetName());
	ensureMsgf(CloseSound, TEXT("CloseSound 가 유효하지 않습니다. [%s]"), *GetName());
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
		if (!HasAuthority() && IsValid(OpenSound))
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}
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

void AFCDoor::OnTimelineFinished()
{
	if (HasAuthority()) return;

	if(!bIsOpen)
	{
		if (!IsValid(CloseSound)) return;
		UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
	}
}


