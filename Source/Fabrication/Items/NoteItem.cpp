#include "Items/NoteItem.h"
#include "Items/Data/NoteData.h"
#include "Player/FCPlayerCharacter.h"
#include "GameState/FCGameState.h"
#include "Engine/DataTable.h"
#include "Controller/FCPlayerController.h"

ANoteItem::ANoteItem()
{
	// 기본 ItemID를 "Note"로 설정
	ItemID = FName(TEXT("Note"));
}

void ANoteItem::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	// GameState 가져오기 (모든 플레이어 공유)
	AFCGameState* GameState = GetWorld()->GetGameState<AFCGameState>();
	if (!IsValid(GameState)) return;

	// 쪽지 데이터 확인 (로깅용)
	AFCPlayerController* PC = Cast<AFCPlayerController>(Cast<AFCPlayerCharacter>(User)->GetController());
	if (!PC) return;
	if (!PC->NoteDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("NoteItem: NoteDataTable이 설정되지 않았습니다."));
	}

	int32 Noteid = GameState->GetRandomNote();
	UE_LOG(LogTemp, Error, TEXT(" ExecuteServerLogic NoteID: %d"), Noteid);
	if (Noteid == -1) return;

	// GameState에 쪽지 번호 추가 (모든 플레이어가 공유)
	GameState->AddCollectedNote(Noteid);

	// TODO: 여기서 모든 클라이언트에게 쪽지 내용을 표시하는 UI를 띄울 수 있습니다.
	// 예: Player->Client_ShowNote(NoteData->NoteText, NoteData->bIsLying);
	PC->ClientRPC_ShowNote(Noteid);

	Destroy();
}

const FNoteData* ANoteItem::GetNoteData() const
{
	if (!NoteDataTable)
	{
		return nullptr;
	}

	FName RowName = GetNoteRowName();
	return NoteDataTable->FindRow<FNoteData>(RowName, TEXT("NoteItem::GetNoteData"));
}

FName ANoteItem::GetNoteRowName() const
{
	// NoteID를 문자열로 변환하여 Row Name으로 사용
	// DataTable에서 Row Name은 "1", "2", "3" 같은 형식이 될 것입니다.
	return FName(*FString::Printf(TEXT("Note_%d"), NoteID-1));
}

