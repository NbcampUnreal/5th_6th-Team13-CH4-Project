#pragma once
#include "Engine/DataTable.h"
#include "NoteData.generated.h"

USTRUCT(BlueprintType)
struct FNoteData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Context;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTruth;
};