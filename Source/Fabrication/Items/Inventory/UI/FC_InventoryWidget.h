#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FC_InventoryWidget.generated.h"

UCLASS()
class FABRICATION_API UFC_InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//로컬 UI 상태 
	UPROPERTY(BlueprintReadWrite)
	int32 SelectQuickSlotIndex = INDEX_NONE;
	
	//구현 = Blueprint / 호출 = C++ / Blueprint 
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_SetQuickSlotSelection(int32 NewIndex);
};
