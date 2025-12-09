#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FCGameInstance.generated.h"

UCLASS()
class FABRICATION_API UFCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	FString NickName;
	
	UFUNCTION(BlueprintCallable)
	void SetNickName(const FString& NewName);
};
