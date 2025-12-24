#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FCGameState_Lobby.generated.h"

UCLASS()
class FABRICATION_API AFCGameState_Lobby : public AGameStateBase
{
	GENERATED_BODY()
public:
	// Replicated 변수 - 모든 플레이어 준비 완료 여부
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bAllPlayersReady;
    
	// 모든 플레이어 준비 상태 체크 (서버에서 호출)
	void CheckAllPlayersReady();
	
	bool IsAllPlayersReady() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
