#include "Player/FCPlayerCharacter.h"
#include "Controller/FCPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/Components/SpeedControlComponent.h"
#include "Flash/FlashLight.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

AFCPlayerCharacter::AFCPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh(), TEXT("head"));
	Camera->bUsePawnControlRotation = true;
	
	SpeedControlComp = CreateDefaultSubobject<USpeedControlComponent>(TEXT("SpeedControl"));

	bUseFlashLight = false;
}

// Called when the game starts or when spawned
void AFCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitalizeFlashLight();


}

void AFCPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AFCPlayerController* FCPC = Cast<AFCPlayerController>(Controller))
		{
			EnInputComp->BindAction(FCPC->MoveAction, ETriggerEvent::Triggered, this, &AFCPlayerCharacter::Move);
			EnInputComp->BindAction(FCPC->LookAction, ETriggerEvent::Triggered, this, &AFCPlayerCharacter::Look);
			EnInputComp->BindAction(FCPC->ItemUseAction, ETriggerEvent::Started, this, &AFCPlayerCharacter::ItemUse);
			EnInputComp->BindAction(FCPC->Interact, ETriggerEvent::Started, this, &AFCPlayerCharacter::Interaction);
		}
	}
}

void AFCPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentAimPitch);
	DOREPLIFETIME(ThisClass, bUseFlashLight);
}

void AFCPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(Controller))
	{
		PrevAimPitch = CurrentAimPitch;

		FRotator ControlRot = Controller->GetControlRotation();
		float NormalizedPitch = FRotator::NormalizeAxis(ControlRot.Pitch);
		CurrentAimPitch = FMath::Clamp(NormalizedPitch, -70.f, 70.f);
	}

	if (IsLocallyControlled() && PrevAimPitch != CurrentAimPitch)
	{
		ServerRPCUpdateAimPitch(CurrentAimPitch);
	}
}

void AFCPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller)
	{
		return;
	}

	const FVector2D MoveVec = Value.Get<FVector2D>();


	if (!FMath::IsNearlyZero(MoveVec.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveVec.X);
	}

	if (!FMath::IsNearlyZero(MoveVec.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveVec.Y);
	}
}

void AFCPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVec = Value.Get<FVector2D>();

	AddControllerYawInput(LookVec.X);
	AddControllerPitchInput(LookVec.Y);
}

void AFCPlayerCharacter::ItemUse(const FInputActionValue& Value)
{
	// 물약 사용 관련하여 테스트를 위해 추가함
	ServerRPCPlayMontage();
	PlayMontage();
}

void AFCPlayerCharacter::Interaction(const FInputActionValue& Value)
{
	// 상호 작용
}

// 기능 분리를 위해 ActorComponent에서 처리 하도록 구현하도록 하였으나
// 상황에 따라 ActorComponent 제거 후 캐릭터 내부에서 처리 할 가능성 있음
void AFCPlayerCharacter::UpdateSpeedByHP(int32 CurHP)
{
	if (IsValid(SpeedControlComp))
	{
		SpeedControlComp->SetSpeedByHealth(CurHP);
	}
}

void AFCPlayerCharacter::PlayMontage()
{
	if (IsValid(DrinkMontage))
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(DrinkMontage);
		}
	}
}

void AFCPlayerCharacter::InitalizeFlashLight()
{
	if (FlashLigthClass)
	{
		FActorSpawnParameters Params;

		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FlashLightInstance = GetWorld()->SpawnActor<AFlashLight>(
			FlashLigthClass,
			Params
		);

		FlashLightInstance->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("FlashLight"));
		
		FlashLightInstance->SetActorHiddenInGame(true);
		FlashLightInstance->SetActorEnableCollision(false);
	}
}

void AFCPlayerCharacter::OnPlayerDiedProcessing()
{
	// Controller의 PlayerDie에 대한 처리 함수 호출
}

void AFCPlayerCharacter::ServerRPCChangeUseFlashLightValue_Implementation()
{
	bUseFlashLight = !bUseFlashLight;
	FlashLightInstance->SetActorHiddenInGame(!bUseFlashLight);
	FlashLightInstance->SetActorEnableCollision(bUseFlashLight);
}

void AFCPlayerCharacter::ServerRPCPlayMontage_Implementation()
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (IsValid(PlayerController) && Controller != PlayerController)
		{
			AFCPlayerCharacter* OtherCharacter = Cast<AFCPlayerCharacter>(PlayerController->GetPawn());
			if (IsValid(OtherCharacter))
			{
				OtherCharacter->ClientRPCPlayMontage(this);
			}
		}
	}
}

void AFCPlayerCharacter::ClientRPCPlayMontage_Implementation(AFCPlayerCharacter* TargetCharacter)
{
	if (IsValid(TargetCharacter))
	{
		TargetCharacter->PlayMontage();
	}
}

void AFCPlayerCharacter::ServerRPCUpdateAimPitch_Implementation(float AimPitchValue)
{
	CurrentAimPitch = AimPitchValue;
}



