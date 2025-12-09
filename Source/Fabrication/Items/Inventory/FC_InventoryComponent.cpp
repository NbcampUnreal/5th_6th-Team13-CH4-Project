#include "Items/Inventory/FC_InventoryComponent.h"
#include "Net/UnrealNetwork.h"

UFC_InventoryComponent::UFC_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


bool UFC_InventoryComponent::AddItem(FName ID, int32 Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return false;
	return true; 
}

void UFC_InventoryComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFC_InventoryComponent, Inventory);
}

void UFC_InventoryComponent::OnRep_Inventory()
{
	//UI,HUD,사운드 재생, 이펙트 등 
}


