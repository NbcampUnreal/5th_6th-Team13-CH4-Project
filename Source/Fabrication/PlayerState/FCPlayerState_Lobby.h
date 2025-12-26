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
	UFUNCTION()
	void OnRep_PlayerNickName();

private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerNickName)
	FString PlayerNickName;
	
#pragma region Ready
	
public:
	
	UPROPERTY(ReplicatedUsing = OnRep_bIsReady, BlueprintReadOnly)
	uint8 bIsReady : 1 = false;

	UFUNCTION()
	void OnRep_bIsReady();

	void SetReady(bool bReady);
	
	bool IsReady() const;
	
#pragma endregion
};
