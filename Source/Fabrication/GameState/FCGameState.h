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
	
	void SetKeyCollected(int32 KeyIndex);
	
	void CheckAllKeysCollected();
	
	void CheckCanEscape();
};
