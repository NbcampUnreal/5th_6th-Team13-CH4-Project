#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "FC_InventoryComponent.generated.h"

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
	
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	int32 InvSize = 15; 

public:
	bool AddItem(const FName& id, int32 count=1);
	void UseItem(const FName& id);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	const TArray<FInventoryItem>& GetInventory() const;
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 GetInvSize() const; 

public:
	UFUNCTION() 
	void OnRep_Inventory();

};
