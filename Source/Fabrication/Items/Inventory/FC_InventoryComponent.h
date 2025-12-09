#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "FC_InventoryComponent.generated.h"

struct FItemData;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FABRICATION_API UFC_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFC_InventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FName ItemID; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ItemCount; 

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<FName> Inventory;

	bool AddItem(FName ItemID, int32 Amount = 1);

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UFUNCTION() 
	void OnRep_Inventory();

};
