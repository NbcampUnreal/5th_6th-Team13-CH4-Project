#include "Items/Inventory/FC_InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"

UFC_InventoryComponent::UFC_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	//Inventory 4칸 
	Inventory.SetNum(InvSize);
	//QuickSlot 4칸 
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
			OnRep_Inventory();
			OnRep_QuickSlot();
			return true;
		}
	}
	//인벤토리 꽉 참 
	return false; 
}

bool UFC_InventoryComponent::AssignQuickSlot(int32 SlotIndex, int32 InvIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return false;

	if (!Inventory.IsValidIndex(InvIndex) || !QuickSlots.IsValidIndex(SlotIndex)) return false;

	const FInventoryItem& InvItem = Inventory[InvIndex];

	if (InvItem.ItemID == NAME_None || InvItem.ItemCount <= 0) return false;

	QuickSlots[SlotIndex] = InvIndex;

	OnRep_QuickSlot();

	return true;
}

void UFC_InventoryComponent::UseItem(const FName& id)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner()))
	{
		if (id == "HealingItem")
		{
			//힐 아이템 사용 효과 
			UE_LOG(LogTemp, Warning, TEXT("Use Heal Item"));
		}
		if (id == "RevivalItem")
		{
			//소생 아이템 사용 효과 
		}
	}
}
void UFC_InventoryComponent::DropAllItems()
{
	AActor* Owner = GetOwner(); 
	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Owner))
	{
		//Player->HP == 0이면 템 다 떨구기 1~4 슬롯 아이템 DropItem() 
	}
}
//캐릭터에서 Key 바인딩 함수 UseQuIckSlot1~UseQuickSlot4 호출 -> ItemID에 따라 UseItem에서 처리 
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
		OnRep_QuickSlot();
		return false; 
	}

	UseItem(SlotItem.ItemID);

	SlotItem.ItemCount--;
	UE_LOG(LogTemp, Warning, TEXT("After  Use: Slot=%d InvIndex=%d Count=%d"),
		SlotIndex, InvIndex, SlotItem.ItemCount);
	if (SlotItem.ItemCount <= 0)
	{
		SlotItem.ItemID = NAME_None;
		SlotItem.ItemCount = 0;
		QuickSlots[SlotIndex] = INDEX_NONE;
	}

	OnRep_Inventory();
	OnRep_QuickSlot();

	return true;
}


void UFC_InventoryComponent::OnRep_Inventory()
{
	//Inventory UI,HUD,사운드 재생, 이펙트 등 
	OnInventoryUpdated.Broadcast();
}

void UFC_InventoryComponent::OnRep_QuickSlot()
{
	//QuickSlot UI, HUD 갱신, 사운드 재생 
	OnInventoryUpdated.Broadcast();
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

