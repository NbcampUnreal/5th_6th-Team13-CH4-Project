#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FCPlayerState_Lobby.generated.h"

UCLASS()
class FABRICATION_API AFCPlayerState_Lobby : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetPlayerNickName(const FString& InNickName);
	const FString& GetPlayerNickName() const;

private:
	UPROPERTY(Replicated)
	FString PlayerNickName;
};
