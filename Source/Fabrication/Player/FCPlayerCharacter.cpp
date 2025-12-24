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
#include "Items/Inventory/UI/FC_InventoryWidget.h"
#include "Items/HealingItem.h"
#include "Animation/FCAnimInstance.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Controller/FCSpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Sound/SoundCue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Flash/UI/FC_FlashLightBattery.h"

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
	
	NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));

	bUseFlashLight = false;
	bFlashLightOn = false; 
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
			EnInputComp->BindAction(FCPC->DropAction, ETriggerEvent::Started, this, &AFCPlayerCharacter::Drop);
			EnInputComp->BindAction(FCPC->OnFlashLight, ETriggerEvent::Started, this, &AFCPlayerCharacter::ToggleFlashLight);
		}
	}
}

void AFCPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentAimPitch);
	DOREPLIFETIME(ThisClass, bUseFlashLight);
	DOREPLIFETIME(ThisClass, FlashLightInstance);
	DOREPLIFETIME(ThisClass, bFlashLightOn);
	DOREPLIFETIME(ThisClass, bFlashTransition);
	DOREPLIFETIME(ThisClass, bPendingUseFlashLight);
	DOREPLIFETIME(ThisClass, bFlashLightUseAble);
	DOREPLIFETIME(ThisClass, CurrentBattery);
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

	if (HasAuthority())
	{
		if (bFlashLightOn && bFlashLightUseAble)
		{
			CurrentBattery -= (DrainRate * DeltaTime);
			CurrentBattery = FMath::Max(0.0f, CurrentBattery);

			if (CurrentBattery <= 0.0f)
			{
				bFlashLightUseAble = false; 
				RemoveFlashLight();
			}
		}
		if (CurrentBattery > 0.0f && !bFlashLightUseAble)
		{
			bFlashLightUseAble = true; 
		}
	}

	if (IsLocallyControlled())
	{
		AFCPlayerController* PC = Cast<AFCPlayerController>(GetController());
		if (!PC) return;
		UFC_InventoryWidget* UI = Cast<UFC_InventoryWidget>(PC->InvInstance);
		if (!UI) return;

		int32 InvIndex = UI->UseQuickSlotIndex;
		if (!InvenComp->Inventory.IsValidIndex(InvIndex)) return;

		const FName ItemId = InvenComp->Inventory[InvIndex].ItemID;
		if (ItemId != "RevivalItem") return;

		DrawReviveRangeCycle(GetWorld(), GetActorLocation(), 300.0f);
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
//Only Server Function
void AFCPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (HasAuthority())
	{
		InitalizeAttachItem();
		ClientRPCFlashLightSetting();//Server -> 소유 클라에 FlashLight 세팅 요청 
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
	// 여기 키는 무엇으로??
	//UsePoitionAction();

	if (!GetController() || !InvenComp) return;
	if (!IsLocallyControlled()) return;

	AFCPlayerController* PC = Cast<AFCPlayerController>(GetController());
	if (!PC || PC->bDropMode) return;

	UFC_InventoryWidget* UI = Cast<UFC_InventoryWidget>(PC->InvInstance);
	if (!UI) return;

	int32 InvIndex = UI->UseQuickSlotIndex;
	if (!InvenComp->Inventory.IsValidIndex(InvIndex)) return;

	//서버에 InvIndex(Select Slot State) 변경 RPC 요청
	InvenComp->Server_RequestUseItem(InvIndex);
}

void AFCPlayerCharacter::Interaction(const FInputActionValue& Value)
{
	// 상호 작용
	//ChangeUseFlashLightValue(!bUseFlashLight);
	//OnPlayerDiedProcessing();
	EnableLineTrace();

	//PlayMontage(EMontage::Die);
	//OnPlayerDiePreProssessing(); //ServerRPC 해당 플레이어 죽음 처리
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

void AFCPlayerCharacter::Drop(const FInputActionValue& value)
{
	AFCPlayerController* PC = Cast<AFCPlayerController>(GetController());
	if (!PC || !PC->bDropMode || !InvenComp) return;
	
	UFC_InventoryWidget* UI = Cast<UFC_InventoryWidget>(PC->InvInstance);
	if (!UI) return;
	
	const int32 SlotIndex = UI->SelectQuickSlotIndex;
	if (SlotIndex == INDEX_NONE) return;

	const TArray<int32> Slots = InvenComp->GetQuickSlots();
	if (!Slots.IsValidIndex(SlotIndex)) return;
	
	const int32 InvIndex = Slots[SlotIndex];
	if (InvIndex == INDEX_NONE) return; 

	//서버로 Inventory배열 InvIndex의 아이템을 드랍한다. RPC요청 보내기 
	InvenComp->Server_RequestDropItem(InvIndex);
	
}

void AFCPlayerCharacter::ToggleFlashLight(const FInputActionValue& value)
{
	if (!IsLocallyControlled()) return;
	if (!bUseFlashLight) return;
	if (bFlashTransition) return;

	ServerRPCToggleFlashLight();
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

void AFCPlayerCharacter::PlayMontage(EMontage MontageType)
{
	const int32 Index = static_cast<int32>(MontageType);
	if (!PlayerMontages.IsValidIndex(Index)) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	UAnimMontage* Montage = PlayerMontages[Index];
	if (!Montage) return;

	if (AnimInstance->Montage_IsPlaying(Montage)) return;

	const float Result = AnimInstance->Montage_Play(Montage);
	if (Result <= 0.0f) return;
}

void AFCPlayerCharacter::InitalizeAttachItem()
{
	if (FlashLigthClass && HasAuthority())
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FlashLightInstance = GetWorld()->SpawnActor<AFlashLight>(
			FlashLigthClass,
			Params
		);
		if (FlashLightInstance)
		{
			FlashLightInstance->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
				TEXT("FlashLight"));
			FlashLightInstance->SetActorHiddenInGame(true);
			FlashLightInstance->SetActorEnableCollision(false);
			FlashLightInstance->AttachSettingFlashLight();
		}
	}

	if (HealItemClass && HasAuthority())
	{
		FActorSpawnParameters Params;

		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		HealItemInstance = GetWorld()->SpawnActor<AHealingItem>(
			HealItemClass,
			Params
		);

		HealItemInstance->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("PotionSocket"));

		HealItemInstance->SetActorHiddenInGame(true);
		HealItemInstance->SetActorEnableCollision(false);
	}
}

void AFCPlayerCharacter::OnPlayerDiedProcessing()
{
	if (AFCPlayerController* FCPC = Cast<AFCPlayerController>(Controller))
	{
		if (IsLocallyControlled())
		{
			FCPC->OnDieProcessing();
		}
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

void AFCPlayerCharacter::UseQuickSlotItem(int32 SlotIndex)
{
	AFCPlayerController* PC = Cast<AFCPlayerController>(GetController());
	if (!PC || !InvenComp) return;

	UFC_InventoryWidget* UI = Cast<UFC_InventoryWidget>(PC->InvInstance);
	if (!UI) return;

	const TArray<int32> QuickSlots = InvenComp->GetQuickSlots();
	if (!QuickSlots.IsValidIndex(SlotIndex)) return;

	const int32 InvIndex = QuickSlots[SlotIndex];
	
	const bool bHasItem = 
		InvIndex != INDEX_NONE &&
		InvenComp->Inventory.IsValidIndex(InvIndex) &&
		InvenComp->Inventory[InvIndex].ItemID != NAME_None &&
		InvenComp->Inventory[InvIndex].ItemCount > 0;

	if (PC->bDropMode)
	{
		if (bHasItem)
		{
			//중복 키 방지 
			if (UI->SelectQuickSlotIndex == SlotIndex)
			{
				UI->SelectQuickSlotIndex = INDEX_NONE;
				UI->BP_SetQuickSlotSelection(INDEX_NONE);
				PC->RemoveDescription();
				return;
			}
			UI->SelectQuickSlotIndex = SlotIndex; 
			UI->BP_SetQuickSlotSelection(SlotIndex); //Quick Slot Select State
			PC->RequestShowDescription(InvenComp->Inventory[InvIndex].ItemID);
		}
		else
		{
			//빈 슬롯 
			UI->SelectQuickSlotIndex = INDEX_NONE;
			UI->BP_SetQuickSlotSelection(INDEX_NONE);
			PC->RemoveDescription();
			SetAttachItem(EAttachItem::None, true);
		}
		return;
	}
	//Normal Mode - Use Item 
	if (bHasItem && IsLocallyControlled())
	{
		if (UI->UseQuickSlotIndex == InvIndex)
		{
			UI->UseQuickSlotIndex = INDEX_NONE;
			UI->BP_SetQuickSlotSelection(INDEX_NONE);
			PC->RemoveDescription();
			SetAttachItem(EAttachItem::None, true);
			return;
		}
		UI->UseQuickSlotIndex = InvIndex; //will use inventory index 
		UI->BP_SetQuickSlotSelection(SlotIndex);//Quick Slot Select State
		PC->RequestShowDescription(InvenComp->Inventory[InvIndex].ItemID);

		Server_UseQuickSlot(SlotIndex);
		CurrentSelectSlotIndex = SlotIndex;
		return;

	}

	UI->UseQuickSlotIndex = INDEX_NONE;
	UI->BP_SetQuickSlotSelection(INDEX_NONE);
	PC->RemoveDescription();

	return;
}

void AFCPlayerCharacter::UsePotionAction()
{
	ServerRPCPlayMontage(EMontage::Drinking);
	PlayMontage(EMontage::Drinking);
}

void AFCPlayerCharacter::FootStepAction()
{
	UE_LOG(LogTemp, Warning, TEXT("FootStepAction"));
	//MakeNoise(0.1f, Cast<APawn>(this), GetActorLocation(), 0.0f, FName("Lure"));
	// 사운드 출력
	if (FootStepSoundAttenuation && FootStepSound && IsLocallyControlled())
	{
		PlayFootStepSound(GetActorLocation(), GetActorRotation());
		
		UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),
		1.0f,      // Loudness
		this,      // Instigator
		0.0f,      // MaxRange (0 = 무제한)
		NAME_None  // Tag -> FName("FootStep") ??
		);
		ServerRPCPlayFootStep(GetActorLocation(), GetActorRotation());
	}
}

void AFCPlayerCharacter::OnPlayerDiePreProssessing()
{
	if (!Controller) return;

	Controller->SetIgnoreMoveInput(true);//죽었을 때 입력 차단

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_None);
		MovementComp->StopMovementImmediately(); //즉시 이동 정지 + 
		MovementComp->DisableMovement(); //이동 비활성화 + 
		MovementComp->SetComponentTickEnabled(false); // Tick중지 
	}
	//서버에서 PlayerState 업데이트 
	if (HasAuthority())
	{
		ServerRPCPlayerDieProcessing();
	}

	PlayMontage(EMontage::Die);

	//로컬 플레이어인 경우 관전모드 전환 
	if (IsLocallyControlled())
	{
		OnPlayerDiedProcessing();
	}
}

// 손전등을 실제로 Use 인풋을 통해서 사용했을 때
void AFCPlayerCharacter::UseFlashLight()
{
	if (HasAuthority())
	{
		bUseFlashLight = !bUseFlashLight;
	}
}

void AFCPlayerCharacter::SetAttachItem(EAttachItem AttachItem, bool bSetHidden)
{
	if (HasAuthority())
	{
		if (AttachItem == EAttachItem::FlashLight)
		{
			FlashLightInstance->SetVisibilityPickupItem(bSetHidden);
			HealItemInstance->SetVisibilityPickupItem(true);
			FlashLightInstance->SetActorEnableCollision(false);
		}
		else if (AttachItem == EAttachItem::Potion)
		{
			HealItemInstance->SetVisibilityPickupItem(bSetHidden);
			if (!bUseFlashLight)
			{
				FlashLightInstance->SetVisibilityPickupItem(true);
			}
			HealItemInstance->SetActorEnableCollision(false);
		}
		else if (AttachItem == EAttachItem::None)
		{
			if (!bUseFlashLight)
			{
				FlashLightInstance->SetVisibilityPickupItem(true);
			}
			HealItemInstance->SetVisibilityPickupItem(true);
		}
	}
}

void AFCPlayerCharacter::CheckingSelectSlot()
{
	if (InvenComp)
	{
		const TArray<FInventoryItem>&  Inven = InvenComp->GetInventory();
		if (Inven[CurrentSelectSlotIndex].ItemCount <= 0)
		{
			InvenComp->ServerRPCAttachItemSetting(Inven[CurrentSelectSlotIndex].ItemID);
		}
	}
}

void AFCPlayerCharacter::OnRep_UsingFlashLight()
{
	ChangeUseFlashLightValue(bUseFlashLight);

	if (InvenComp)
	{
		InvenComp->HandleInventoryUpdated();
	}

	if (IsLocallyControlled())
	{
		AFCPlayerController* PC = Cast<AFCPlayerController>(GetController());
		if (!PC) return;

		if (bUseFlashLight)
		{
			PC->CreateBatteryWidget();
		}
		else
		{
			PC->RemoveBatteryWidget();
		}
	}
}

void AFCPlayerCharacter::PlayFootStepSound(FVector Location, FRotator Rotation)
{
	UGameplayStatics::PlaySoundAtLocation(
		this,
		FootStepSound,
		Location,
		Rotation,
		1.0f,
		1.0f,
		0.0f,
		FootStepSoundAttenuation
		);
}

void AFCPlayerCharacter::OnRep_FlashLightOn()
{
	if (FlashLightInstance)
	{
		FlashLightInstance->AttachSettingFlashLight();
		FlashLightInstance->SetVisibilitySpotLight(bFlashLightOn);
	}
}

void AFCPlayerCharacter::Server_UseQuickSlot_Implementation(int32 Index)
{
	if (InvenComp)
	{
		InvenComp->UseQuickSlot(Index);
	}
}

void AFCPlayerCharacter::ClientRPCFlashLightSetting_Implementation()
{
	if (FlashLightInstance)
	{
		FlashLightInstance->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("FlashLight"));
		FlashLightInstance->AttachSettingFlashLight();
		ChangeUseFlashLightValue(bUseFlashLight);
	}
}

void AFCPlayerCharacter::ChangeUseFlashLightValue(bool bIsUsing)
{
	if (FlashLightInstance)
	{
		FlashLightInstance->AttachSettingFlashLight();
		FlashLightInstance->SetActorHiddenInGame(!bIsUsing);
		FlashLightInstance->SetVisibilitySpotLight(bIsUsing && bFlashLightOn);
		FlashLightInstance->SetActorEnableCollision(false); //손으로 들면 Collision 끄기 
	}
}
float AFCPlayerCharacter::GetBatteryPercent() const
{
	return CurrentBattery / MaxBattery;
}
float AFCPlayerCharacter::GetCurrentBattery() const
{
	return CurrentBattery;
}
bool AFCPlayerCharacter::IsFlashLightUseAble() const
{
	return bFlashLightUseAble;
}


void AFCPlayerCharacter::RemoveFlashLight()
{
	if (!HasAuthority()) return;
	if (!InvenComp) return;
	if (bFlashLightUseAble) return;

	bUseFlashLight = false; 
	OnRep_UsingFlashLight();
	
	bFlashLightOn = false; 
	OnRep_FlashLightOn();

	int32 FlashLightInvIndex = INDEX_NONE;

	const TArray<int32>& QuickSlots = InvenComp->GetQuickSlots();
	const TArray<FInventoryItem>& Inventory = InvenComp->GetInventory();

	for (int32 i = 0; i < QuickSlots.Num(); ++i) {
		int32 InvIndex = QuickSlots[i];
		if (InvIndex != INDEX_NONE && Inventory.IsValidIndex(InvIndex))
		{
			if (Inventory[InvIndex].ItemID == TEXT("FlashLight")) 
			{
				FlashLightInvIndex = InvIndex;
				break;
			}
		}
	}
	if (FlashLightInvIndex != INDEX_NONE) {
		InvenComp->RemoveItem(FlashLightInvIndex);
	}
}
void AFCPlayerCharacter::DrawReviveRangeCycle(UWorld* World, const FVector PlayerLocation, float Radius)
{
	if(!World) return;

	const FVector Center = PlayerLocation + FVector(0.f, 0.f, 2.0f);
	
	DrawDebugCircle(
		World, Center, Radius, 60, FColor::Cyan, false, 0.0f, 0, 0.5f, FVector(1, 0, 0), FVector(0, 1, 0), false
	);
}
void AFCPlayerCharacter::ServerRPCChangeUseFlashLightValue_Implementation(bool bIsUsing)
{
	bUseFlashLight = bIsUsing;
	OnRep_UsingFlashLight();
}

void AFCPlayerCharacter::ServerRPCToggleFlashLight_Implementation()
{
	if (!bFlashLightUseAble || !bUseFlashLight) return;

	bFlashLightOn = !bFlashLightOn;
	OnRep_FlashLightOn();
}

void AFCPlayerCharacter::ServerRPCChangeOnFlashLightValue_Implementation(bool bFlashOn)
{
	bFlashLightOn = bFlashOn;
	OnRep_FlashLightOn();
}

void AFCPlayerCharacter::ServerRPCPlayMontage_Implementation(EMontage MontageType)
{
	MulticastRPCPlayMontage(MontageType);
}

void AFCPlayerCharacter::ClientRPCPlayMontage_Implementation(AFCPlayerCharacter* TargetCharacter, EMontage MontageType)
{
	if (IsValid(TargetCharacter))
	{
		TargetCharacter->PlayMontage(MontageType);
	}
}

void AFCPlayerCharacter::ServerRPCUpdateAimPitch_Implementation(float AimPitchValue)
{
	CurrentAimPitch = AimPitchValue;
}

void AFCPlayerCharacter::ServerRPCInteract_Implementation(AActor* TargetActor, ACharacter* User,
	const FHitResult& HitResult)
{
	if (IInteractable* Interface = Cast<IInteractable>(TargetActor))
	{
		Interface->ExecuteServerLogic(User, HitResult);
	}
}

void AFCPlayerCharacter::ServerRPCPlayerDieProcessing_Implementation()
{
	if (AFCPlayerState* FCPS = Cast<AFCPlayerState>(GetPlayerState()))
	{
		FCPS->bIsDead = true;
	}
}

void AFCPlayerCharacter::ClientRPCSelfPlayMontage_Implementation(EMontage Montage)
{
	PlayMontage(Montage); 
}

void AFCPlayerCharacter::ServerRPCPlayFootStep_Implementation(FVector Location, FRotator Rotation)
{
	MulticastRPCPlayFootStep(Location, Rotation);
}

void AFCPlayerCharacter::MulticastRPCPlayFootStep_Implementation(FVector Location, FRotator Rotation)
{
	if (!IsLocallyControlled())
	{
		PlayFootStepSound(Location, Rotation);
	}
}

void AFCPlayerCharacter::MulticastRPCPlayMontage_Implementation(EMontage MontageType)
{
	PlayMontage(MontageType);
}

void AFCPlayerCharacter::ServerToggleEquipFlashlight_Implementation()
{
	if (!HasAuthority()) return;
	if (bFlashTransition) return; //Montage Playing  

	bFlashTransition = true;
	bPendingUseFlashLight = !bUseFlashLight; //Mext Montage 

	const EMontage UseMontage = bPendingUseFlashLight ? EMontage::RaiseFlashLight : EMontage::LowerFlashLight;

	MulticastRPCPlayMontage(UseMontage);

	const int32 Index = (int32)UseMontage;
	if (PlayerMontages.IsValidIndex(Index) && PlayerMontages[Index])
	{
		const float MontageLength = PlayerMontages[Index]->GetPlayLength();

		// FlashEquip/UnEquip 타이밍
		float SwitchTime = 0.0f;
		if (UseMontage == EMontage::RaiseFlashLight)
		{
			SwitchTime = MontageLength * 0.15f; //15%
		}
		else
		{
			SwitchTime = MontageLength * 0.85f; //85%
		}

		// FlashTransitionEnd 타이밍
		const float EndTime = MontageLength * 0.95f; //95%

		FTimerHandle SwitchTimer, EndHandleTimer;

		GetWorldTimerManager().SetTimer(
			SwitchTimer,
			[this, UseMontage]()
			{
				if (UseMontage == EMontage::RaiseFlashLight)
				{
					MulticastRPC_FlashEquip();
				}
				else
				{
					MulticastRPC_FlashUnEquip();
				}
			},
			SwitchTime,
			false
		);

		GetWorldTimerManager().SetTimer(
			EndHandleTimer,
			[this]()
			{
				MulticastRPC_FlashTransitionEnd();
			},
			EndTime,
			false
		);
	}
}

void AFCPlayerCharacter::MulticastRPC_FlashEquip_Implementation()
{
	if (HasAuthority())
	{
		bUseFlashLight = true;
	}
	OnRep_UsingFlashLight(); // 모든 클라이언트에서 실행
}

void AFCPlayerCharacter::MulticastRPC_FlashUnEquip_Implementation()
{
	if (HasAuthority())
	{
		bUseFlashLight = false;
		if (bFlashLightOn)
		{
			bFlashLightOn = false;
			OnRep_FlashLightOn();
		}
	}
	OnRep_UsingFlashLight();
}

void AFCPlayerCharacter::MulticastRPC_FlashTransitionEnd_Implementation()
{
	if (HasAuthority())
	{
		bFlashTransition = false;
	}
}

void AFCPlayerCharacter::PlayerReviveProcessing()
{
	if(!HasAuthority()) return;
	
	AFCPlayerState* FCPS = GetPlayerState<AFCPlayerState>();
	if (!FCPS || !FCPS->bIsDead) return; //죽지 않은 경우 
	
	if (StatusComp)
	{
		StatusComp->SetCurrentHP(1);//소생 시 1로 부활 
	}

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_Walking);//이동 제한 해제 
		MovementComp->SetComponentTickEnabled(true);
		MovementComp->StopMovementImmediately();
	}

	if(Controller) Controller->SetIgnoreMoveInput(false);//이동 입력 제한 해제 
	
	//Controller 재 빙의 
	//if (AFCPlayerController* PC = Cast<AFCPlayerController>(GetController()))
	//{
	//	//관전 Pawn -> 소생 Pawn으로 빙의 
	//	if (PC->FCSpectatorPawn)
	//	{
	//		PC->FCSpectatorPawn->Destroy();
	//		PC->FCSpectatorPawn = nullptr;
	//	}

	//	if (PC->GetPawn() != this)
	//	{
	//		PC->UnPossess();
	//		PC->Possess(this);
	//	}
	//}

	//에니메이션 추가? 
	//MulticastRPC_PlayReviveAnimation();
}

void AFCPlayerCharacter::MulticastRPC_ReviveAnimation_Implementation()
{
}
