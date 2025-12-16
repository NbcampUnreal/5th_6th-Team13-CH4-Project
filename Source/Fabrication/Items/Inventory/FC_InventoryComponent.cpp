#include "Items/Inventory/FC_InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Items/PickupItemBase.h"
#include "Items/Data/ItemData.h"
#include "Controller/FCPlayerController.h"

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
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("[AddItem] Failed - No Authority"));
		return false;
	}
	if (count <= 0 || id == NAME_None) return false; 

	UE_LOG(LogTemp, Warning, TEXT("[AddItem] Owner=%s Role=%d id=%s count=%d"),
		*GetOwner()->GetName(),
		(int32)GetOwner()->GetLocalRole(),
		*id.ToString(),
		count);

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
					UE_LOG(LogTemp, Warning, TEXT("AddItem in For Loop Is Atvie Index: %d"), s);
					break;
				}
			}
			HandleInventoryUpdated();
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
void UFC_InventoryComponent::DropItem(int32 Index)
{
	UE_LOG(LogTemp, Error, TEXT("DropItem Ative"));
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
	//APickupItem - ItemID를 찾아 엑터 스폰 
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!ItemDataTable) return;

	const FItemData* RowName = ItemDataTable->FindRow<FItemData>(id, TEXT("No DropRowID"));
	if (!RowName || !RowName->DropActorClass) return; 
	
	UWorld* World = GetWorld(); 
	if (!World) return;
	//GetOwner() => Inventory가 붙어있는 FCPlayerCharacter 반환(Type=AActor) 
	FVector Loc = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
	FRotator Rot = GetOwner()->GetActorRotation();

	FActorSpawnParameters Parms;
	Parms.Owner = GetOwner(); 
	Parms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APickupItemBase* SpawnDropItem = World->SpawnActor<APickupItemBase>(RowName->DropActorClass, Loc, Rot, Parms);
	if (SpawnDropItem)
	{
		UE_LOG(LogTemp, Error, TEXT("Success SpawnActor"));
	}
	return;
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
		return false; 
	}
	UE_LOG(LogTemp, Warning, TEXT("Before  Use: Slot=%d InvIndex=%d Count=%d"),
		SlotIndex, InvIndex, SlotItem.ItemCount);

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
	HandleInventoryUpdated();

	return true;
}


void UFC_InventoryComponent::OnRep_Inventory()
{
	//Inventory UI,HUD,사운드 재생, 이펙트 등 
	UE_LOG(LogTemp, Warning, TEXT("[Client] OnRep_Inventory Called"));
	HandleInventoryUpdated();
}

void UFC_InventoryComponent::OnRep_QuickSlot()
{
	//QuickSlot UI, HUD 갱신, 사운드 재생 
	UE_LOG(LogTemp, Warning, TEXT("[Client] OnRep_QuickSlot Called"));
	HandleInventoryUpdated();
}

void UFC_InventoryComponent::HandleInventoryUpdated()
{
	AActor* Owner = GetOwner();
	FString OwnerName = Owner ? Owner->GetName() : TEXT("NoOwner");
	ENetRole LocalRole = Owner ? Owner->GetLocalRole() : ROLE_None;

	UE_LOG(LogTemp, Warning, TEXT("[HandleInventoryUpdated] Owner=%s Role=%d Inv[0].ID=%s QuickSlots[0]=%d"),
		*OwnerName,
		(int32)LocalRole,
		Inventory.Num() > 0 ? *Inventory[0].ItemID.ToString() : TEXT("N/A"),
		QuickSlots.Num() > 0 ? QuickSlots[0] : -999);

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

