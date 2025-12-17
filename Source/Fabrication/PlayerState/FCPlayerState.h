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

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FString& GetPlayerNickName() const;
	
	UPROPERTY(Replicated)
	uint8 bIsReady : 1 = false;

	UPROPERTY(Replicated)
	uint8 bIsDead : 1;
	
	UFUNCTION()
	void OnRep_ChangedPlayerNickName();
	
	void SetPlayerNickName(const FString& NewNickName);
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_ChangedPlayerNickName)
	FString PlayerNickName;

	
};
