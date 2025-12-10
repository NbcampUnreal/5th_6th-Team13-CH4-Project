#include "Items/Inventory/FC_InventoryComponent.h"
#include "Net/UnrealNetwork.h"

UFC_InventoryComponent::UFC_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	Inventory.SetNum(InvSize);
}

void UFC_InventoryComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UFC_InventoryComponent, Inventory, COND_OwnerOnly);
}

bool UFC_InventoryComponent::AddItem(const FName& id, int32 count)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return false;
	
	if (count <= 0 || id == NAME_None) return false; 

	for (auto& Inv : Inventory)
	{
		if (Inv.ItemID == NAME_None)
		{
			Inv.ItemID = id;
			Inv.ItemCount = count;
			OnRep_Inventory();
			return true;
		}
	}
	//인벤토리 꽉 참 
	return false; 
}

void UFC_InventoryComponent::UseItem(const FName& id)
{
}

const TArray<FInventoryItem>& UFC_InventoryComponent::GetInventory() const
{
	return Inventory; 
}

int32 UFC_InventoryComponent::GetInvSize() const
{
	return InvSize;
}

void UFC_InventoryComponent::OnRep_Inventory()
{
	//UI,HUD,사운드 재생, 이펙트 등 
}


