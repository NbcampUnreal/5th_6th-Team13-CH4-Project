#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FCSessionManager.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCreateSessionComplete);
DECLARE_MULTICAST_DELEGATE(FOnFindSessionComplete);
DECLARE_MULTICAST_DELEGATE(FOnJoinSessionComplete);
DECLARE_MULTICAST_DELEGATE(FOnDestroySessionComplete);
UCLASS()
class FABRICATION_API UFCSessionManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void CreateSession();
	UFUNCTION(BlueprintCallable)
	void FindSession();
	UFUNCTION(BlueprintCallable)
	void JoinSession();
	UFUNCTION(BlueprintCallable)
	void DestroySession();
	
	FOnCreateSessionComplete OnCreateSessionComplete;
	FOnFindSessionComplete OnFindSessionComplete;
	FOnJoinSessionComplete OnJoinSessionComplete;
	FOnDestroySessionComplete OnDestroySessionComplete;
};
