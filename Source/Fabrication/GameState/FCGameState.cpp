#include "GameState/FCGameState.h"
#include "Net/UnrealNetwork.h"
#include "Data/FCKeyType.h"
#include "Controller/FCPlayerController.h"

void AFCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, bCanEscape);
	DOREPLIFETIME(ThisClass, KeyIndex);
	DOREPLIFETIME(ThisClass, CollectedNoteIDs);
	DOREPLIFETIME(ThisClass, RemainGameTime);

}

void AFCGameState::OnRep_OnKeyCollected()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("OnRepKey %d"), KeyIndex));
}

void AFCGameState::SetKeyCollected()
{
	if (!HasAuthority())
	{
		return;
	}
	
	++KeyIndex;

	if (KeyIndex >= RequiredKey)
	{
		bCanEscape = true;
	}

	OnCanEscape.Broadcast(bCanEscape);
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

void AFCGameState::SetRequiredKey(int32 InKey)
{
	RequiredKey = InKey;
}

int32 AFCGameState::GetCurrKey() const
{
	return KeyIndex; 
}

void AFCGameState::InitializeNote()
{
	TotalNoteID.Reset();
	TotalNoteID.Reserve(14);

	for (int i = 1; i <= 14; ++i)
	{
		TotalNoteID.Add(i);
	}
	for (int i = TotalNoteID.Num() - 1; i > 0; --i)
	{
		const int j = FMath::RandRange(0, i);
		TotalNoteID.Swap(i, j);
	}
}

int32 AFCGameState::GetRandomNote()
{
	if (!HasAuthority() || TotalNoteID.Num() == 0) return -1;
	
	const int32 NoteID = TotalNoteID[0];
	TotalNoteID.RemoveAt(0);

	return NoteID;
}

bool AFCGameState::HasCollectedNote(int32 NoteID) const
{
	return CollectedNoteIDs.Contains(NoteID);
}

void AFCGameState::AddCollectedNote(int32 NoteID)
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("Begin AddCollectedNote!!!!!!! NoteID: %d"),NoteID);

	if (!CollectedNoteIDs.Contains(NoteID))
	{
		CollectedNoteIDs.Add(NoteID);

		UE_LOG(LogTemp, Log, TEXT("쪽지 %d 획득 (팀 전체 공유)"), NoteID);
		OnRep_CollectedNotes();
	}
}

void AFCGameState::OnRep_CollectedNotes()
{
	// 쪽지 획득 시 UI 업데이트 등 처리 가능
	UE_LOG(LogTemp, Log, TEXT("획득한 쪽지 수: %d"), CollectedNoteIDs.Num());
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AFCPlayerController* FCPC = Cast<AFCPlayerController>(PC))
		{
			// SharedNote UI 업데이트
			FCPC->UpdateSharedNoteUI();

			UE_LOG(LogTemp, Log, TEXT("로컬 플레이어의 SharedNote UI 업데이트 요청"));
		}
	}
}

void AFCGameState::SetRemainGameTime(int32 InTime)
{
	if (!HasAuthority()) return;
	
	RemainGameTime = InTime;
}

void AFCGameState::OnRep_RemainGameTime()
{
	// 클라이언트에서 RemainGameTime이 업데이트될 때 호출됨
	UE_LOG(LogTemp, Log, TEXT("클라이언트: RemainGameTime 업데이트: %d"), RemainGameTime);
}

