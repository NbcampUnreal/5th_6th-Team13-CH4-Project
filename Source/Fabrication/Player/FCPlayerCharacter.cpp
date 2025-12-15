#include "Player/FCPlayerCharacter.h"
#include "Controller/FCPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/Components/SpeedControlComponent.h"
#include "Flash/FlashLight.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Items/Inventory/FC_InventoryComponent.h"
#include "Items/PickupItemBase.h"
#include "Fabrication.h"
#include "PlayerState/FCPlayerState.h"
#include "Player/Components/StatusComponent.h"
#include "Engine/DamageEvents.h"
#include "Items/Interface/ItemInterface.h"


AFCPlayerCharacter::AFCPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh(), TEXT("head"));
	Camera->bUsePawnControlRotation = true;
	
	SpeedControlComp = CreateDefaultSubobject<USpeedControlComponent>(TEXT("SpeedControl"));
	InvenComp = CreateDefaultSubobject<UFC_InventoryComponent>(TEXT("InvenComp"));

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	StatusComp = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));

	bUseFlashLight = false;
	LineTraceDist = 1000.0f;
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
			EnInputComp->BindAction(FCPC->ItemUseAction, ETriggerEvent::Started, this, &AFCPlayerCharacter::ItemUse);
			EnInputComp->BindAction(FCPC->Interact, ETriggerEvent::Started, this, &AFCPlayerCharacter::Interaction);
			EnInputComp->BindAction(FCPC->FirstQuickSlot, ETriggerEvent::Started, this, &AFCPlayerCharacter::UseItemSlot1);
			EnInputComp->BindAction(FCPC->SecondQuickSlot, ETriggerEvent::Started, this, &AFCPlayerCharacter::UseItemSlot2);
			EnInputComp->BindAction(FCPC->ThirdQuickSlot, ETriggerEvent::Started, this, &AFCPlayerCharacter::UseItemSlot3);
			EnInputComp->BindAction(FCPC->FourthQuickSlot, ETriggerEvent::Started, this, &AFCPlayerCharacter::UseItemSlot4);
			EnInputComp->BindAction(FCPC->DropMode, ETriggerEvent::Started, this, &AFCPlayerCharacter::ToggleDropMode);
		}
	}
}

void AFCPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentAimPitch);
	DOREPLIFETIME(ThisClass, bUseFlashLight);
	DOREPLIFETIME(ThisClass, FlashLightInstance);
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

	if (IsLocallyControlled())
	{
		if (PrevAimPitch != CurrentAimPitch)
		{
			ServerRPCUpdateAimPitch(CurrentAimPitch);
		}
	}
}

float AFCPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 데미지 적용
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (IsValid(StatusComp))
	{
		StatusComp->ApplyDamage(static_cast<int32>(ActualDamage));
	}

	return ActualDamage;
}

void AFCPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (HasAuthority())
	{
		InitalizeFlashLight();
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
	//ServerRPCChangeUseFlashLightValue(!bUseFlashLight);
}

void AFCPlayerCharacter::Interaction(const FInputActionValue& Value)
{
	// 상호 작용
	//ServerRPCChangeUseFlashLightValue(!bUseFlashLight);
	OnPlayerDiedProcessing();
	//FDamageEvent DamageEvent;
	//TakeDamage(1.0f, DamageEvent, nullptr, this);
	//EnableLineTrace();
}

void AFCPlayerCharacter::UseItemSlot1(const FInputActionValue& Value)
{
	UseQuickSlotItem(0);
}

void AFCPlayerCharacter::UseItemSlot2(const FInputActionValue& Value)
{
	UseQuickSlotItem(1);
}

void AFCPlayerCharacter::UseItemSlot3(const FInputActionValue& Value)
{
	UseQuickSlotItem(2);
}

void AFCPlayerCharacter::UseItemSlot4(const FInputActionValue& Value)
{
	UseQuickSlotItem(3);
}

void AFCPlayerCharacter::ToggleDropMode(const FInputActionValue& value)
{
	if (!IsLocallyControlled()) return;

	if (AFCPlayerController* PC = Cast<AFCPlayerController>(GetController()))
	{
		PC->ToggleDropMode();
	}
}

void AFCPlayerCharacter::Server_AssignQuickSlot_Implementation(int32 SlotIndex, int32 InvIndex)
{
	if(!InvenComp) return;
	InvenComp->AssignQuickSlot(SlotIndex, InvIndex);
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
	// Controller의 PlayerDie에 대한 처리 함수 호출 (테스트중)
	if (AFCPlayerController* FCPC = Cast<AFCPlayerController>(Controller))
	{
		FCPC->OnDieProcessing();
	}
}

void AFCPlayerCharacter::EnableLineTrace()
{
	FHitResult HitResult;

	FVector StartPos = Camera->GetComponentLocation();
	FVector EndPos = StartPos + (Camera->GetForwardVector() * LineTraceDist);
	DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Green, false, 5.0f);

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECC_PickUp);

	if (bIsHit)
	{
		if (IInteractable* Item = Cast<IInteractable>(HitResult.GetActor()))
		{
			Item->Interact(this, HitResult);
		}
	}
}

void AFCPlayerCharacter::UseQuickSlotItem(int32 Index)
{
	AFCPlayerController* PC = Cast<AFCPlayerController>(GetController());
	if (!PC || !InvenComp) return;
	
	if (PC->bDropMode)
	{
		const TArray<int32>& Slots = InvenComp->GetQuickSlots();
		if (!Slots.IsValidIndex(Index)) return;

		const int32 InvIndex = Slots[Index];
		if (InvIndex == INDEX_NONE) return;

		InvenComp->Server_RequestDropItem(InvIndex);
		return;
	}
	//Not DropMode
	if (HasAuthority())
	{
		InvenComp->UseQuickSlot(Index);
	}
	else
	{
		//Client -> Server RPC 요청 
		Server_UseQuickSlot(Index);
	}
	return;
}

void AFCPlayerCharacter::Server_UseQuickSlot_Implementation(int32 Index)
{
	if (InvenComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Server RPC] UseQuickSlotItem %d"), Index);
		InvenComp->UseQuickSlot(Index);
	}
}

void AFCPlayerCharacter::ClientRPCFlashLightSetting_Implementation()
{
	if (FlashLightInstance)
	{
		FlashLightInstance->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("FlashLight"));
	}
}

void AFCPlayerCharacter::ServerRPCChangeUseFlashLightValue_Implementation(bool bIsUsing)
{
	bUseFlashLight = bIsUsing;
	FlashLightInstance->SetActorHiddenInGame(!bIsUsing);
	FlashLightInstance->SetActorEnableCollision(bIsUsing);

	FlashLightInstance->UsingFlashLight();
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



