#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItemBase.h"
#include "NoteItem.generated.h"

UCLASS()
class FABRICATION_API ANoteItem : public APickupItemBase
{
	GENERATED_BODY()

public:
	ANoteItem();

	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;

	// 쪽지 번호 (이 쪽지가 어떤 쪽지인지 식별)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	int32 NoteID = 0;

	// 쪽지 데이터를 가져올 DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	UDataTable* NoteDataTable = nullptr;

	const struct FNoteData* GetNoteData() const;

private:
	// NoteID를 FName으로 변환 (DataTable에서 찾기 위해)
	FName GetNoteRowName() const;
};

