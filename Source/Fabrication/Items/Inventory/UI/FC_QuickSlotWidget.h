#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "FC_QuickSlotWidget.generated.h"

class UFC_InventoryComponent;

UCLASS()
class FABRICATION_API UFC_QuickSlotWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget),BlueprintReadOnly, Category = "Slot")
	UImage* QuickSlotIcon;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Slot")
	UTextBlock* ItemCountText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	int32 SlotIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	UFC_InventoryComponent* InvenComp = nullptr;
	UPROPERTY(meta = (BindWidget))
	UBorder* SlotBorder;

	UPROPERTY(BlueprintReadWrite,Category="Slot")
	bool bHasItem = false; 

public:
	UFUNCTION(BlueprintCallable, Category = "Slot")
	void InitializeSlot(int32 InSlotIndex, UFC_InventoryComponent* InventoryComp);

	UFUNCTION(BlueprintCallable, Category = "Slot")
	void UpdateSlotUI();

	//OnDragCancelled Delegate Binding Function 
	UFUNCTION()
	void HandleDragCancelled(UDragDropOperation* Operation);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void BP_UpdateSlotEffect();

protected:
	UFUNCTION()
	FEventReply OnSlotBorderMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	virtual void NativeOnDragEnter(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	virtual void NativeOnDragLeave(
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

private:
	FVector2D DragStartPos;
};
