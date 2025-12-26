#pragma once

#include "Engine/DataTable.h"
#include "NoteData.generated.h"

USTRUCT(BlueprintType)
struct FNoteData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 쪽지 번호 (고유 ID)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	int32 NoteID;

	// 쪽지에 적힌 문구 (게임 규칙)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note", meta = (MultiLine = true))
	FText NoteText;

	// 이 쪽지가 거짓말인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	bool bIsLying;

	FNoteData()
		: NoteID(0)
		, NoteText(FText::GetEmpty())
		, bIsLying(false)
	{
	}
};

