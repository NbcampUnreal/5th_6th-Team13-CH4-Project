#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FCGameState.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Waiting,
	Playing,
	Ending,
	End
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCanEscape, bool);

UCLASS()
class FABRICATION_API AFCGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override; 
	void SetKeyCollected();
	void CheckCanEscape();
	bool CanEscape();
	void SetRequiredKey(int32 InKey);
	
	// 게임 시간 관련 함수
	void SetRemainGameTime(int32 InTime);
	int32 GetRemainGameTime() const { return RemainGameTime; }
	
	// 대기 시간 관련 함수
	void SetRemainWaitingTime(int32 InTime);
	int32 GetRemainWaitingTime() const { return RemainWaitingTime; }
	
	int32 GetCurrKey() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPlayGameTime() const;

	UFUNCTION()
	void OnRep_OnKeyCollected();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	EMatchState MatchState = EMatchState::Waiting;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 RemainGameTime = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 TotalGameTime = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 RemainWaitingTime = 0;
	
	FOnCanEscape OnCanEscape;

	UPROPERTY(EditDefaultsOnly, Category = "Note")
	UDataTable* NoteDataTable = nullptr;

public:
	UFUNCTION()
	void InitializeNote();
	void BuildNoteIdPoolsFromDataTable();

	UFUNCTION()
	int32 GetRandomNote();
	// 쪽지를 획득했는지 확인하는 함수
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Note")
	bool HasCollectedNote(int32 NoteID) const;

	// 쪽지를 획득 처리하는 함수 (서버에서 호출)
	void AddCollectedNote(int32 NoteID);

	// 획득한 쪽지 번호들 (모든 플레이어 공유)
	UPROPERTY(ReplicatedUsing = OnRep_CollectedNotes, BlueprintReadOnly, Category = "Note", meta = (AllowPrivateAccess))
	TArray<int32> CollectedNoteIDs;

	UPROPERTY(EditDefaultsOnly, Category = "Note")
	TArray<int32> TrueNoteIDs;

	UPROPERTY(EditDefaultsOnly, Category = "Note")
	TArray<int32> FalseNoteIDs;

	UPROPERTY(EditDefaultsOnly, Category = "Note", meta = (ClampMin = "0"))
	int32 Number_FalseNote = 7; 

	TArray<int32> NewNotePool;
	
private:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 AlivePlayerControllerCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess))
	uint8 bCanEscape : 1;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_OnKeyCollected, Category = "Key", meta = (AllowPrivateAccess))
	int32 KeyIndex = 0;
	int32 RequiredKey;

	//True + False Notes 
	UPROPERTY()
	TArray<int32> TotalNoteID;

	UFUNCTION()
	void OnRep_CollectedNotes();
};
