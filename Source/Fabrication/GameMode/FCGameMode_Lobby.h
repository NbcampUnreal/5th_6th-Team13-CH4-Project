#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FCGameMode_Lobby.generated.h"

UCLASS()
class FABRICATION_API AFCGameMode_Lobby : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFCGameMode_Lobby();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void SendChatMessage(const FString& Message);

private:
	UPROPERTY()
	TArray<TObjectPtr<APlayerController>> PlayerControllers;
};
