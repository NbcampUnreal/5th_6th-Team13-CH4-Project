// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/FCSpectatorPawn.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

AFCSpectatorPawn::AFCSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;
	SetReplicateMovement(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	RootComponent = SpringArm;

	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
}

void AFCSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AFCSpectatorPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFCSpectatorPawn, SpectateTarget);
}

void AFCSpectatorPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsValid(SpectateTarget))
	{
		return;
	}

	const FVector TargetLocation =
		SpectateTarget->GetActorLocation() + FollowOffset;

	const FVector NewLocation = FMath::VInterpTo(
		GetActorLocation(),
		TargetLocation,
		DeltaSeconds,
		LocationInterpSpeed
	);

	SetActorLocation(NewLocation);
}

void AFCSpectatorPawn::SetSpectateTarget(APawn* InTarget)
{
	SpectateTarget = InTarget;
}

void AFCSpectatorPawn::OnRep_SpectateTarget()
{
	if (!IsValid(SpectateTarget)) return;

	// 즉시 스냅
	const FVector InitLocation =
		SpectateTarget->GetActorLocation() + FollowOffset;

	SetActorLocation(InitLocation);
}




