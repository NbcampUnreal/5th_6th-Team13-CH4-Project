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

public:	
	UFC_InventoryComponent();

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<FInventoryItem> Inventory;

	//인벤토리 인덱스 저장 
	UPROPERTY(ReplicatedUsing = OnRep_QuickSlot)
	TArray<int32> QuickSlots;
	
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	//Inv & Slot 델리게이트 
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

private:
	int32 InvSize = 4; 

public:
	bool AddItem(const FName& id, int32 count=1);
	void UseItem(const FName& id);
	void DropAllItems();
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool UseQuickSlot(int32 SlotIndex);

	//QuickSlot Index <-> Inventory Index 연동 
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AssignQuickSlot(int32 SlotIndex, int32 InvIndex);

public:
	UFUNCTION(BlueprintCallable, Category="Inventory")
	const TArray<FInventoryItem>& GetInventory() const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<int32>& GetQuickSlots() const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 GetInvSize() const; 

public:
	UFUNCTION() 
	void OnRep_Inventory();
	UFUNCTION()
	void OnRep_QuickSlot(); 
};
