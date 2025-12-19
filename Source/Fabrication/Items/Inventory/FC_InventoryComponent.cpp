#include "Items/Inventory/FC_InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Items/PickupItemBase.h"
#include "Items/Data/ItemData.h"
#include "Controller/FCPlayerController.h"
#include "Items/Inventory/UI/FC_InventoryWidget.h"

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
			Player->ClientRPCSelfPlayMontage(EMontage::Drinking);
			UE_LOG(LogTemp, Warning, TEXT("Use Heal Item"));
		}
		else if (id == "RevivalItem")
		{
			//Revival Effect 
		}
		else if (id == "FlashLight")
		{
			Player->UseFlashLight();
		}
	}
}
void UFC_InventoryComponent::DropAllItems()
{
	AActor* Owner = GetOwner(); 
	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Owner))
	{
		//Player->HP == 0�̸� �� �� ������ 1~4 ���� ������ DropItem() 
	}
}
void UFC_InventoryComponent::DropItem(int32 Index)
{
	int32 InvIndex = Index;
	if (!Inventory.IsValidIndex(InvIndex)) return;
	if (Inventory[InvIndex].ItemID == NAME_None || Inventory[InvIndex].ItemCount <= 0) return;

	UE_LOG(LogTemp, Warning, TEXT("[%d] Before DropItemID: %s, Count:%d"),InvIndex, *Inventory[InvIndex].ItemID.ToString(),Inventory[InvIndex].ItemCount);
	Inventory[InvIndex].ItemCount--;
	UE_LOG(LogTemp, Warning, TEXT("[%d] After DropItemID: %s, Count:%d"),InvIndex, *Inventory[InvIndex].ItemID.ToString(), Inventory[InvIndex].ItemCount);
	if (Inventory[InvIndex].ItemCount <= 0)
	{
		Inventory[InvIndex].ItemCount = 0;
		Inventory[InvIndex].ItemID = NAME_None;
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

	SpawnDroppedItem(Dropid);
	DropItem(InvIndex);
}

void UFC_InventoryComponent::SpawnDroppedItem(const FName& id, int32 count)
{
	//APickupItem - ItemID�� ã�� ���� ���� 
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!ItemDataTable) return;

	const FItemData* RowName = ItemDataTable->FindRow<FItemData>(id, TEXT("No DropRowID"));
	if (!RowName || !RowName->DropActorClass) return; 
	
	UWorld* World = GetWorld(); 
	if (!World) return;

	//GetOwner() => Inventory�� �پ��ִ� FCPlayerCharacter ��ȯ(Type=AActor) 
	FVector Loc = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
	FRotator Rot = GetOwner()->GetActorRotation();

	FActorSpawnParameters Parms;
	Parms.Owner = GetOwner(); 
	Parms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APickupItemBase* SpawnDropItem = World->SpawnActor<APickupItemBase>(RowName->DropActorClass, Loc, Rot, Parms);

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
	SlotItem.ItemCount--;

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
	}
}

void UFC_InventoryComponent::ServerRPCAttachItemSetting_Implementation()
{
	AttachItemSetting(FName("HealingItem"), true);
}


