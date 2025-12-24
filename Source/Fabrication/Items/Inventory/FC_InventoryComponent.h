#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "FC_InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

class AFCPlayerCharacter;

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ItemCount = 0; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	float ItemCondition = 1.0f;
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

	//�κ��丮 �ε��� ���� 
	UPROPERTY(ReplicatedUsing = OnRep_QuickSlot)
	TArray<int32> QuickSlots;
	
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Item")
	UDataTable* ItemDataTable;

public:
	//Inv & Slot ��������Ʈ 
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

private:
	int32 InvSize = 4; 
#pragma endregion

#pragma region Function 
public:
	bool AddItem(const FName& id, int32 count=1);
	void UseItem(const FName& id);

	void DropAlIItems();
	void DropItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseQuickSlot(int32 SlotIndex);

	//QuickSlot Index <-> Inventory Index ���� 
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AssignQuickSlot(int32 SlotIndex, int32 InvIndex);

	UFUNCTION()
	void SpawnDroppedItem(const FName& id, int32 count = 1, float ItemCondition = 1);
	
	void AttachItemSetting(const FName& ItemID, bool bSetHidden);
	
	FVector SpawnItemLineTrace(FVector BaseLocation);
	
	FVector DropItemPositionSetting();

	void RemoveItem(int32 InvIndex);
	//내 주변 죽은 플레이어 탐색 
	UFUNCTION()
	AFCPlayerCharacter* FindDeadPlayer(AFCPlayerCharacter* Player);
	
	UFUNCTION()
	void AlivePlayerProcessing();

#pragma endregion

#pragma region RPC 
public:
	//Client -> UseQuickSlotItem(int32 Index) Server�� RPC ��û  
	UFUNCTION(Server, Reliable,BlueprintCallable)
	void Server_RequestDropItem(int32 InvIndex);

	UFUNCTION(Server, Reliable)
	void Server_RequestUseItem(int32 InvIndex);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestSwapItem(int32 SlotA, int32 SlotB);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCAttachItemSetting(const FName AttachItemName);
	

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

	UFUNCTION()
	void HandleInventoryUpdated();
#pragma endregion
};
