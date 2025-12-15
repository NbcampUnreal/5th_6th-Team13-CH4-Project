#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FCPlayerState.generated.h"

UCLASS()
class FABRICATION_API AFCPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	uint8 bIsReady : 1;

	UPROPERTY(Replicated)
	uint8 bIsDead : 1;
};
