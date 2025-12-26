#include "GameState/FCGameState.h"
#include "Net/UnrealNetwork.h"
#include "Data/FCKeyType.h"

void AFCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, bCanEscape);
	DOREPLIFETIME(ThisClass, KeyIndex);
	DOREPLIFETIME(ThisClass, CollectedNoteIDs);

}

void AFCGameState::OnRep_OnKeyCollected()
{
	UE_LOG(LogTemp, Error, TEXT("OnRepKey %d"), KeyIndex);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("OnRepKey %d"), KeyIndex));
}

void AFCGameState::SetKeyCollected()
{
	if (!HasAuthority())
	{
		return;
	}
	
	++KeyIndex;
	UE_LOG(LogTemp, Error, TEXT("GetKey %d"), KeyIndex)

	if (KeyIndex >= RequiredKey)
	{
		bCanEscape = true;
		UE_LOG(LogTemp, Error, TEXT("CanEscapse"))
	}

}

void AFCGameState::CheckCanEscape()
{
	if (!bCanEscape)
	{
		UE_LOG(LogTemp, Warning, TEXT("키 부족"));
		return;
	}
	
	MatchState = EMatchState::Ending;
}

bool AFCGameState::CanEscape()
{
	if (bCanEscape) return true;
	
	return false;
}

bool AFCGameState::HasCollectedNote(int32 NoteID) const
{
	return CollectedNoteIDs.Contains(NoteID);
}

void AFCGameState::AddCollectedNote(int32 NoteID)
{
	if (!HasAuthority()) return;

	if (!CollectedNoteIDs.Contains(NoteID))
	{
		CollectedNoteIDs.Add(NoteID);
		UE_LOG(LogTemp, Log, TEXT("쪽지 %d 획득 (팀 전체 공유)"), NoteID);
	}
}

void AFCGameState::OnRep_CollectedNotes()
{
	// 쪽지 획득 시 UI 업데이트 등 처리 가능
	UE_LOG(LogTemp, Log, TEXT("획득한 쪽지 수: %d"), CollectedNoteIDs.Num());
}

