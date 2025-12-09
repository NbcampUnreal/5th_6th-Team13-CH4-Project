#include "Player/FCPlayerCharacter.h"
#include "Controller/FCPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AFCPlayerCharacter::AFCPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1인칭 카메라 메쉬 소켓에 붙이는게 이상하기 때문에 C++에서 생성 x
	/*Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh());
	Camera->bUsePawnControlRotation = true;*/
}

// Called when the game starts or when spawned
void AFCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
		}
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

