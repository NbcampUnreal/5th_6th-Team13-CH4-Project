#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "FC_InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ItemCount = 0; 
};

struct FItemData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FABRICATION_API UFC_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
#pragma region Variable
public:	
	UFC_InventoryComponent();

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<FInventoryItem> Inventory;

	//인벤토리 인덱스 저장 
	UPROPERTY(ReplicatedUsing = OnRep_QuickSlot)
	TArray<int32> QuickSlots;
	
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category="Item")
	UDataTable* ItemDataTable;

public:
	//Inv & Slot 델리게이트 
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

private:
	int32 InvSize = 4; 
#pragma endregion

#pragma region Function 
public:
	bool AddItem(const FName& id, int32 count=1);
	void UseItem(const FName& id);

	void DropAllItems();
	void DropItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseQuickSlot(int32 SlotIndex);

	//QuickSlot Index <-> Inventory Index 매핑 
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AssignQuickSlot(int32 SlotIndex, int32 InvIndex);

	UFUNCTION()
	void SpawnDroppedItem(const FName& id, int32 count = 1);
#pragma endregion

#pragma region RPC 
public:
	//Client -> UseQuickSlotItem(int32 Index) Server로 RPC 요청  
	UFUNCTION(Server, Reliable)
	void Server_RequestDropItem(int32 InvIndex);

	UFUNCTION(Server, Reliable)
	void Server_RequestUseItem(int32 InvIndex);
#pragma endregion 

#pragma region Getter
public:
	UFUNCTION(BlueprintCallable, Category="Inventory")
	const TArray<FInventoryItem>& GetInventory() const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<int32>& GetQuickSlots() const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 GetInvSize() const; 
#pragma endregion

#pragma region Delegate
public:
	UFUNCTION() 
	void OnRep_Inventory();
	UFUNCTION()
	void OnRep_QuickSlot(); 

private:
	void HandleInventoryUpdated();
#pragma endregion
};
