#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FCGameInstance.generated.h"

UCLASS()
class FABRICATION_API UFCGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerNickName(const FString& NewName);
	UFUNCTION(BlueprintCallable)
	const FString& GetPlayerNickName() const;

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString PlayernNickName;
	
};
