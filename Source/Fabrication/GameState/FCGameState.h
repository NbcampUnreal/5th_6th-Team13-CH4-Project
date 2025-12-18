#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FCGameState.generated.h"

enum class EFCKeyType : uint8;

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Waiting,
	Playing,
	Ending,
	End
};

UCLASS()
class FABRICATION_API AFCGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 AlivePlayerControllerCount = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EMatchState MatchState = EMatchState::Waiting;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 bGetKey1 : 1;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 bGetKey2 : 1;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 bGetKey3 : 1;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 bCanEscape : 1;
	
	UFUNCTION()
	void OnRep_OnKeyCollected();

	void SetKeyCollected();
	
	void CheckAllKeysCollected();
	
	void CheckCanEscape();

	bool CanEscape();

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_OnKeyCollected)
	int32 KeyIndex = 0;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Key", meta = (AllowPrivateAccess))
	int32 RequiredKey;
};
