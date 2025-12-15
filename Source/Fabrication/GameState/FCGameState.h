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
	uint8 bGetFuse1 : 1;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 bGetFuse2 : 1;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 bGetFuse3 : 1;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 bCanEscape : 1;
	
	void SetFuseCollected(int32 FuseIndex);
	
	void CheckAllFusesCollected();
};
