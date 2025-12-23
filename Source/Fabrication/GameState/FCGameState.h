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

	void SetKeyCollected();
	void CheckCanEscape();
	bool CanEscape();

	UFUNCTION()
	void OnRep_OnKeyCollected();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	EMatchState MatchState = EMatchState::Waiting;

private:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 AlivePlayerControllerCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess))
	uint8 bCanEscape : 1;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_OnKeyCollected, Category = "Key", meta = (AllowPrivateAccess))
	int32 KeyIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Key", meta = (AllowPrivateAccess))
	int32 RequiredKey;
};
