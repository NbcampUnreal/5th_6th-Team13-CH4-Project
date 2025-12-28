#include "Items/Inventory/UI/FC_QuickSlotWidget.h"
#include "Items/Inventory/FC_InventoryComponent.h"
#include "SlotDragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Items/Data/ItemData.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Player/FCPlayerCharacter.h"
#include "Controller/FCPlayerController.h"
#include "Items/Inventory/UI/FC_InventoryWidget.h"

void UFC_QuickSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SlotBorder)
	{
		SlotBorder->OnMouseButtonDownEvent.BindUFunction(this, "OnSlotBorderMouseDown");
	}
	if (InvenComp)
	{
		InvenComp->OnInventoryUpdated.RemoveDynamic(this, &UFC_QuickSlotWidget::UpdateSlotUI);
	}
}

void UFC_QuickSlotWidget::InitializeSlot(int32 InSlotIndex, UFC_InventoryComponent* InventoryComp)
{
	SlotIndex = InSlotIndex;
	InvenComp = InventoryComp;

	if (InvenComp)
	{
		InvenComp->OnInventoryUpdated.AddDynamic(this, &UFC_QuickSlotWidget::UpdateSlotUI);
	}

	UpdateSlotUI();
}

void UFC_QuickSlotWidget::UpdateSlotUI()
{
	if (!InvenComp || !QuickSlotIcon || !ItemCountText) return;

	bHasItem = false;

	const TArray<int32>& QuickSlots = InvenComp->GetQuickSlots();
	const TArray<FInventoryItem>& Inventory = InvenComp->GetInventory();

	if (!QuickSlots.IsValidIndex(SlotIndex)) return;

	const int32 InvIndex = QuickSlots[SlotIndex];
	
	if (InvIndex == INDEX_NONE || !Inventory.IsValidIndex(InvIndex))
	{
		QuickSlotIcon->SetVisibility(ESlateVisibility::Collapsed);
		ItemCountText->SetVisibility(ESlateVisibility::Collapsed);
		ItemCountText->SetText(FText::GetEmpty());

		if (EquipBorder) EquipBorder->SetVisibility(ESlateVisibility::Collapsed);
		if (EquipText) {
			EquipText->SetText(FText::GetEmpty());
			EquipText->SetVisibility(ESlateVisibility::Collapsed);
		}
		BP_UpdateSlotEffect();
		return;
	}

	const FInventoryItem& Item = Inventory[InvIndex];

	if (Item.ItemID == NAME_None || Item.ItemCount <= 0)
	{
		QuickSlotIcon->SetVisibility(ESlateVisibility::Collapsed);
		ItemCountText->SetVisibility(ESlateVisibility::Collapsed);
		ItemCountText->SetText(FText::GetEmpty());

		if (EquipBorder) EquipBorder->SetVisibility(ESlateVisibility::Collapsed);
		if (EquipText){
			EquipText->SetText(FText::GetEmpty());
			EquipText->SetVisibility(ESlateVisibility::Collapsed);
		}
		BP_UpdateSlotEffect();
		return;
	}

	bHasItem = true; 

	if (InvenComp->ItemDataTable)
	{
		const FItemData* ItemData = InvenComp->ItemDataTable->FindRow<FItemData>(Item.ItemID, TEXT(""));

		if (ItemData && ItemData->Icon)
		{
			QuickSlotIcon->SetBrushFromTexture(ItemData->Icon);
			QuickSlotIcon->SetVisibility(ESlateVisibility::Visible);
			ItemCountText->SetText(FText::AsNumber(Item.ItemCount));
			ItemCountText->SetVisibility(ESlateVisibility::Visible);
		}
	}
	UpdateEquipFlashLightShow(InvIndex);
	BP_UpdateSlotEffect();
}

void UFC_QuickSlotWidget::UpdateEquipFlashLightShow(int32 InvIndex)
{
	if (!EquipBorder || !EquipText) return;
	if (!InvenComp) return;

	EquipBorder->SetVisibility(ESlateVisibility::Collapsed);
	EquipText->SetText(FText::GetEmpty());

	const TArray<FInventoryItem>& Inventory = InvenComp->GetInventory();
	if (!Inventory.IsValidIndex(InvIndex)) return;

	const FInventoryItem& Item = Inventory[InvIndex];
	if (Item.ItemID == NAME_None || Item.ItemCount <= 0) return; 

	const FName FlashLightID(TEXT("FlashLight"));
	if (Item.ItemID != FlashLightID) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(InvenComp->GetOwner());
	if (!Player) return;

	AFCPlayerController* PC = Cast<AFCPlayerController>(Player->GetController()); 
	if (!PC) return;

	UFC_InventoryWidget* IWG = Cast<UFC_InventoryWidget>(PC->InvInstance);
	if (!IWG) return;

	if (InvIndex != IWG->UseQuickSlotIndex) return;

	EquipBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	EquipText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	const bool bEquipped = Player->bFlashTransition ? Player->bPendingUseFlashLight : Player->bUseFlashLight;

	//ON | OFF Text Setting 
	EquipText->SetText(bEquipped ? FText::FromString(TEXT("ON")) : FText::FromString(TEXT("OFF")));
	EquipText->SetRenderOpacity(bEquipped ? 1.0f : 0.5f);
	EquipText->SetColorAndOpacity(bEquipped 
		? FLinearColor(1.0f, 1.0f, 1.0f, 1.0f) 
		: FLinearColor(0.6f, 0.6f, 0.6f, 0.45f));

	//ON | OFF Border Setting 
	EquipBorder->SetRenderOpacity(bEquipped ? 0.65f : 0.35f);
	EquipBorder->SetBrushColor(bEquipped 
		? FLinearColor(1.0f, 0.8f, 0.3f, 0.65f) 
		: FLinearColor(0.2f, 0.2f, 0.2f, 0.3f));
}


FEventReply UFC_QuickSlotWidget::OnSlotBorderMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
		return FEventReply(false);

	if (!InvenComp)
		return FEventReply(false);

	const TArray<int32>& QuickSlots = InvenComp->GetQuickSlots();
	const TArray<FInventoryItem>& Inventory = InvenComp->GetInventory();

	if (!QuickSlots.IsValidIndex(SlotIndex))
		return FEventReply(false);

	int32 InvIndex = QuickSlots[SlotIndex];
	if (InvIndex == INDEX_NONE || !Inventory.IsValidIndex(InvIndex))
		return FEventReply(false);

	const FInventoryItem& Item = Inventory[InvIndex];
	if (Item.ItemID == NAME_None || Item.ItemCount <= 0) return FEventReply(false);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton);
}

void UFC_QuickSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!InvenComp) return;

	const TArray<int32>& QuickSlots = InvenComp->GetQuickSlots();
	const TArray<FInventoryItem>& Inventory = InvenComp->GetInventory();

	if (!QuickSlots.IsValidIndex(SlotIndex)) return;

	int32 InvIndex = QuickSlots[SlotIndex];
	if (InvIndex == INDEX_NONE || !Inventory.IsValidIndex(InvIndex)) return;

	USlotDragDropOperation* DragOP = NewObject<USlotDragDropOperation>(this);
	DragOP->SourceSlotIndex = SlotIndex;
	DragOP->SourceInvIndex = QuickSlots[SlotIndex];
	DragOP->ItemID = Inventory[InvIndex].ItemID;

	UImage* DragImage = NewObject<UImage>(DragOP);
	DragImage->SetBrush(QuickSlotIcon->Brush);
	DragImage->SetDesiredSizeOverride(FVector2D(64.f, 64.f));

	DragOP->DefaultDragVisual = DragImage;
	DragOP->Pivot = EDragPivot::CenterCenter;

	DragOP->OnDragCancelled.AddDynamic(this, &UFC_QuickSlotWidget::HandleDragCancelled);

	OutOperation = DragOP;
}

bool UFC_QuickSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	USlotDragDropOperation* DragOP = Cast<USlotDragDropOperation>(InOperation);
	if (!DragOP || !InvenComp) return false;

	const TArray<int32>& QuickSlots = InvenComp->GetQuickSlots();
	if (!QuickSlots.IsValidIndex(SlotIndex)) return false;

	//int32 TargetInvIndex = QuickSlots[SlotIndex];

	if (DragOP->SourceSlotIndex == SlotIndex) return false;

	InvenComp->Server_RequestSwapItem(DragOP->SourceSlotIndex, SlotIndex);

	return true;
}

void UFC_QuickSlotWidget::HandleDragCancelled(UDragDropOperation* Operation)
{
	USlotDragDropOperation* DragOP = Cast<USlotDragDropOperation>(Operation);
	if (!DragOP || !InvenComp) return;

	if (DragOP->SourceInvIndex != INDEX_NONE)
	{
		InvenComp->Server_RequestDropItem(DragOP->SourceInvIndex);
	}
}

void UFC_QuickSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UFC_QuickSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}
