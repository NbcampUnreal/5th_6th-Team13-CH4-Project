#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "SlotDragDropOperation.generated.h"

UCLASS()
class FABRICATION_API USlotDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category="Drag")
	int32 SourceSlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite, Category="Drag")
	int32 SourceInvIndex = INDEX_NONE; 

	UPROPERTY(BlueprintReadWrite, Category="Drag")
	FName ItemID = NAME_None;
};
