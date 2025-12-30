#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FCGameInstance.generated.h"

UCLASS()
class FABRICATION_API UFCGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFCGameInstance();
	
	UFUNCTION(BlueprintCallable)
	void SetLocalPlayerNickName(const FString& NewName);
	UFUNCTION(BlueprintCallable)
	const FString& GetLocalPlayerNickName() const;

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString LocalPlayerNickName;
	
};
