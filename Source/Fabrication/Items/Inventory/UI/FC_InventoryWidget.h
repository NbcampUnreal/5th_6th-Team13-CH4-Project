#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FC_InventoryWidget.generated.h"

UCLASS()
class FABRICATION_API UFC_InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//Drop Select Slot Index 
	UPROPERTY(BlueprintReadWrite)
	int32 SelectQuickSlotIndex = INDEX_NONE;
	
	//Use Item Select Slot Index 
	UPROPERTY(BlueprintReadWrite)
	int32 UseQuickSlotIndex = INDEX_NONE;
	
	//Implement = Blueprint / Call = C++ OR Blueprint 
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_SetQuickSlotSelection(int32 NewIndex);
};
