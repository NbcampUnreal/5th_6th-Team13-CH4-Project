#include "Items/Inventory/FC_InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Items/PickupItemBase.h"
#include "Items/Data/ItemData.h"
#include "Controller/FCPlayerController.h"
#include "Items/Inventory/UI/FC_InventoryWidget.h"
#include "Flash/FlashLight.h"
#include "Player/Components/StatusComponent.h"
#include "Fabrication.h"
#include "Engine/OverlapResult.h"

UFC_InventoryComponent::UFC_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	//Inventory 4ĭ 
	Inventory.SetNum(InvSize);
	//QuickSlot 4ĭ 
	QuickSlots.Init(INDEX_NONE, Inventory.Num());
}

void UFC_InventoryComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UFC_InventoryComponent, Inventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UFC_InventoryComponent, QuickSlots, COND_OwnerOnly);
}

bool UFC_InventoryComponent::AddItem(const FName& id, int32 count)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return false;
	
	if (count <= 0 || id == NAME_None) return false; 
	
	for (int32 i = 0; i < Inventory.Num(); ++i)
	{
		if (Inventory[i].ItemID == NAME_None)
		{
			Inventory[i].ItemID = id;
			Inventory[i].ItemCount = count;

			if (id == FName(TEXT("FlashLight")))
			{
				AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner());
				if (!Player) return false;
				if (Inventory[i].ItemCondition <= 0.0f)
				{
					Inventory[i].ItemCondition = 1.0f;
				}
				Player->CurrentBattery = Inventory[i].ItemCondition * Player->MaxBattery;
			}

			for (int32 s = 0; s < QuickSlots.Num(); ++s)
			{
				if (QuickSlots[s] == INDEX_NONE)
				{
					QuickSlots[s] = i;
					break;
				}
			}
			HandleInventoryUpdated();
			return true;
		}
	}
	//�κ��丮 �� �� 
	return false; 
}

bool UFC_InventoryComponent::AssignQuickSlot(int32 SlotIndex, int32 InvIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return false;

	if (!Inventory.IsValidIndex(InvIndex) || !QuickSlots.IsValidIndex(SlotIndex)) return false;

	const FInventoryItem& InvItem = Inventory[InvIndex];

	if (InvItem.ItemID == NAME_None || InvItem.ItemCount <= 0) return false;

	QuickSlots[SlotIndex] = InvIndex;

	HandleInventoryUpdated();

	return true;
}

void UFC_InventoryComponent::UseItem(const FName& id)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner()))
	{
		if (id == "HealingItem")
		{
			//Heal Effect 
			/*Player->ClientRPCSelfPlayMontage(EMontage::Drinking);*/ /*<= 나만 보이게 */
			Player->MulticastRPCPlayMontage(EMontage::Drinking); /*<= 다른 플레이어 보이게 */
			UStatusComponent* Status = Player->FindComponentByClass<UStatusComponent>();
			if (Status)
			{
				Status->HealHP(1);
			}
		}
		else if (id == "RevivalItem")
		{
			//Revival Effect 
		}
		else if (id == "FlashLight")
		{
			Player->ServerToggleEquipFlashlight();
			HandleInventoryUpdated();
		}
	}
}
void UFC_InventoryComponent::DropAlIItems()
{
	AActor* OwnerActor = GetOwner();
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OwnerActor);
	if (!Player) return;

	if (Player->StatusComp->GetCurrentHP() <= 0)
	{
		for (int i = 0; i < Inventory.Num(); ++i)
		{
			if (Inventory[i].ItemID != NAME_None && Inventory[i].ItemCount > 0)
			{
				const int32 InvIndex = i; 
				Server_RequestDropItem(InvIndex);
			}
		}
	}
}
void UFC_InventoryComponent::DropItem(int32 Index)
{
	int32 InvIndex = Index;
	if (!Inventory.IsValidIndex(InvIndex)) return;
	if (Inventory[InvIndex].ItemID == NAME_None || Inventory[InvIndex].ItemCount <= 0) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner());

	Inventory[InvIndex].ItemCount--;

	if (!Player) return;

	if (Inventory[InvIndex].ItemID == TEXT("FlashLight"))
	{
		Player->bFlashTransition = false;
		Player->bPendingUseFlashLight = false;

		if (Player->bUseFlashLight)
		{
			Player->MulticastRPCPlayMontage(EMontage::LowerFlashLight);

			if (Player->PlayerMontages.IsValidIndex(static_cast<int32>(EMontage::LowerFlashLight)))
			{
				UAnimMontage* LowerMontage = Player->PlayerMontages[static_cast<int32>(EMontage::LowerFlashLight)];
				if (LowerMontage)
				{
					const float MontageLength = LowerMontage->GetPlayLength();

					FTimerHandle DropTimerHandle;
					Player->GetWorldTimerManager().SetTimer(
						DropTimerHandle,
						[Player]() {
							Player->SetAttachItem(EAttachItem::FlashLight, true);
							Player->bUseFlashLight = false;
							if (Player->bFlashLightOn)
							{
								Player->bFlashLightOn = false;
								Player->OnRep_FlashLightOn();
							}
							Player->OnRep_UsingFlashLight();
						}, MontageLength * 0.8f, false
					);
				}
			}
		}
		else
		{
			Player->SetAttachItem(EAttachItem::FlashLight, true);
			Player->bUseFlashLight = false; 
			if (Player->bFlashLightOn)
			{
				Player->bFlashLightOn = false;
				Player->OnRep_FlashLightOn();
			}
			Player->OnRep_UsingFlashLight();
		}
	}

	if (Inventory[InvIndex].ItemCount <= 0)
	{
		Inventory[InvIndex].ItemCount = 0;
		Inventory[InvIndex].ItemID = NAME_None;
		Player->SetAttachItem(EAttachItem::None, true);
	}

	for (int32 s = 0; s < QuickSlots.Num(); ++s)
	{
		if (QuickSlots[s] == InvIndex)
		{
			QuickSlots[s] = INDEX_NONE;
		}
	}

	InvIndex = INDEX_NONE;
	HandleInventoryUpdated();
}

void UFC_InventoryComponent::Server_RequestDropItem_Implementation(int32 InvIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!Inventory.IsValidIndex(InvIndex)) return;

	const FInventoryItem& Item = Inventory[InvIndex];
	const FName Dropid = Inventory[InvIndex].ItemID;
	if (Dropid == NAME_None || Inventory[InvIndex].ItemCount <= 0) return;
	//드랍 -> GetOwner 소유자 = nullptr 

	float BatteryPercent = 1.0f; 
	if (Dropid == FName(TEXT("FlashLight")))
	{
		AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner());
		if (Player)
		{
			BatteryPercent = Player->GetBatteryPercent();
		}
	}

	SpawnDroppedItem(Dropid, 1, BatteryPercent);
	DropItem(InvIndex);
}

void UFC_InventoryComponent::SpawnDroppedItem(const FName& id, int32 count, float ItemCondition)
{
	//APickupItem - ItemID�� ã�� ���� ���� 
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!ItemDataTable) return;

	const FItemData* RowName = ItemDataTable->FindRow<FItemData>(id, TEXT("No DropRowID"));
	if (!RowName || !RowName->DropActorClass) return; 
	
	UWorld* World = GetWorld(); 
	if (!World) return;

	//GetOwner() => Inventory�� �پ��ִ� FCPlayerCharacter ��ȯ(Type=AActor) 
	//FVector Loc = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
	FVector Loc = DropItemPositionSetting(); // 앞에 벽 같은 구조물 있을 때 보정을 위해 사용
	FRotator Rot = GetOwner()->GetActorRotation();

	FActorSpawnParameters Parms;
	Parms.Owner = nullptr; 
	Parms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = SpawnItemLineTrace(Loc);
	APickupItemBase* SpawnDropItem = World->SpawnActor<APickupItemBase>(RowName->DropActorClass, SpawnLocation, Rot, Parms);

	//Drop -> Spawn Actor Collision Setting On  
	if (SpawnDropItem)
	{
		if (id == FName(TEXT("FlashLight")))
		{
			SpawnDropItem->StoredBatteryPercent = ItemCondition;
		}
		SpawnDropItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		SpawnDropItem->SetActorHiddenInGame(false);
		SpawnDropItem->SetActorEnableCollision(true);
		
		if (UStaticMeshComponent* StaticMesh = SpawnDropItem->FindComponentByClass<UStaticMeshComponent>())
		{
			StaticMesh->SetCollisionProfileName(TEXT("PickUp"));
		}
		
		if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(SpawnDropItem->GetRootComponent()))
		{
			PrimitiveComp->SetSimulatePhysics(true);
			
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, PrimitiveComp]()
			{
				PrimitiveComp->SetSimulatePhysics(false);
			}),2.0f, false);
			
		}
	}
	return;
}

//Selection State is Currently UI-Local + Validates Slot/Index after local selection and UI Update.
//=> This Function has no major gameplay role yet. 
//Can be Extended If The Server Needs Selected-Slot state.
bool UFC_InventoryComponent::UseQuickSlot(int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return false; 

	if (!QuickSlots.IsValidIndex(SlotIndex)) return false; 

	int32 InvIndex = QuickSlots[SlotIndex];
	if (!Inventory.IsValidIndex(InvIndex)) return false; 

	FInventoryItem& SlotItem = Inventory[InvIndex];
	
	if (SlotItem.ItemID == NAME_None || SlotItem.ItemCount <= 0)
	{
		QuickSlots[SlotIndex] = INDEX_NONE;
		return false; 
	}
	
	AttachItemSetting(SlotItem.ItemID, false);
	return true;
}

void UFC_InventoryComponent::Server_RequestUseItem_Implementation(int32 InvIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!Inventory.IsValidIndex(InvIndex)) return;
	
	FInventoryItem& SlotItem = Inventory[InvIndex];
	if (SlotItem.ItemID == NAME_None || SlotItem.ItemCount <= 0) return;

	UseItem(SlotItem.ItemID);
	
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner());
	if (!Player) return;

 	if (SlotItem.ItemID == TEXT("FlashLight"))
	{
		if (Player->CurrentBattery >= 0.0f)
		{
			return;
		}
	}
	else
	{
		Inventory[InvIndex].ItemCount--;
	}
	AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwner()->GetInstigatorController());
	if (!PC) return;
	PC->RemoveDescription();

	if (SlotItem.ItemCount <= 0)
	{
		SlotItem.ItemCount = 0;
		SlotItem.ItemID = NAME_None;
		for (int32 i = 0; i < QuickSlots.Num(); ++i)
		{
			if (QuickSlots[i] == InvIndex)
			{
				QuickSlots[i] = INDEX_NONE;
			}
		}
		
	}
	HandleInventoryUpdated();
}

void UFC_InventoryComponent::OnRep_Inventory()
{
	//Inventory UI,HUD,���� ���, ����Ʈ �� 
	HandleInventoryUpdated();
}

void UFC_InventoryComponent::OnRep_QuickSlot()
{
	//QuickSlot UI, HUD ����, ���� ��� 
	HandleInventoryUpdated();
}

void UFC_InventoryComponent::HandleInventoryUpdated()
{
	OnInventoryUpdated.Broadcast();
}

void UFC_InventoryComponent::Server_RequestSwapItem_Implementation(int32 SlotA, int32 SlotB)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!QuickSlots.IsValidIndex(SlotA) || !QuickSlots.IsValidIndex(SlotB)) return;
	if (SlotA == SlotB) return;
	UE_LOG(LogTemp, Warning, TEXT("SlotA: %d | SlotB: %d"), SlotA, SlotB);
	QuickSlots.Swap(SlotA, SlotB);
	HandleInventoryUpdated();
}

//Getter() 
const TArray<FInventoryItem>& UFC_InventoryComponent::GetInventory() const
{
	return Inventory;
}

const TArray<int32>& UFC_InventoryComponent::GetQuickSlots() const
{
	return QuickSlots;
}

int32 UFC_InventoryComponent::GetInvSize() const
{
	return InvSize;
}

// 포션 선택 시 포션을 들고 있도록 함
void UFC_InventoryComponent::AttachItemSetting(const FName& ItemID, bool bSetHidden)
{
	if (AFCPlayerCharacter* FCPlayerCharacter = Cast<AFCPlayerCharacter>(GetOwner()))
	{
		if (ItemID == "HealingItem")
		{
			FCPlayerCharacter->SetAttachItem(EAttachItem::Potion, bSetHidden);
		}
		else if (ItemID == "FlashLight")
		{
			FCPlayerCharacter->SetAttachItem(EAttachItem::FlashLight, bSetHidden);
		}
		else if (ItemID == NAME_None)
		{
			FCPlayerCharacter->SetAttachItem(EAttachItem::None, bSetHidden);
		}
	}
}

FVector UFC_InventoryComponent::SpawnItemLineTrace(FVector BaseLocation)
{
	const FVector TraceStart = BaseLocation;
	const FVector TraceEnd = BaseLocation - FVector(0.f, 0.f, 1000.f);

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_PickUp);

	FVector FinalSpawnLocation = BaseLocation;

	bool bIsHit = GetWorld()->LineTraceSingleByObjectType(
		Hit, TraceStart, TraceEnd, ObjectParams, QueryParams);

	if (bIsHit && IsValid(Hit.GetActor())) // 위치 보정을 위해 사용
	{
		FVector GroundLocation = Hit.ImpactPoint;

		// 만약 맞은 것이 기존 아이템이면, 바닥을 다시 탐색
		if (Hit.GetActor()->IsA<APickupItemBase>())
		{
			FVector RetryStart = GroundLocation + FVector(0.0f, 0.0f, 10.0f);
			FVector RetryEnd = RetryStart - FVector(0, 0, 200.0f);

			FHitResult GroundHit;
			bool bGroundHit = GetWorld()->LineTraceSingleByObjectType(
				GroundHit, RetryStart, RetryEnd, ObjectParams, QueryParams);

			if (bGroundHit)
			{
				GroundLocation = GroundHit.ImpactPoint;
			}
		}
	}
	
	return FinalSpawnLocation;
}
FVector UFC_InventoryComponent::DropItemPositionSetting()
{
	FVector Loc;
	FVector ForwardVec = GetOwner()->GetActorForwardVector();
	FVector StartPos = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	FVector EndPos = StartPos + ForwardVec * 100.0f;
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartPos, EndPos, ECC_Visibility, Params))
	{
		// 벽에 부딪혔을 경우 맞은 지점에서 조금 앞으로
		FVector ImpactPoint = Hit.ImpactPoint;
		FVector SpawnLoc = ImpactPoint - ForwardVec * 10.0f; // 약간 뒤로
		Loc = SpawnLoc;
	}
	else
	{
		Loc = EndPos;
	}
	
	return Loc;
}

void UFC_InventoryComponent::ServerRPCAttachItemSetting_Implementation(const FName AttachItemName)
{
	AttachItemSetting(AttachItemName, true);
}


