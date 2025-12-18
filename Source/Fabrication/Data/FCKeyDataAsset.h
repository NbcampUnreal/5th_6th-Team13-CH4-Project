#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/FCKeyType.h"
#include "FCKeyDataAsset.generated.h"

UCLASS(BlueprintType)
class FABRICATION_API UFCKeyDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const EFCKeyType& GetKeyID() const { return KeyID; }
	FText GetDisplayName() const { return DisplayName; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Key")
	EFCKeyType KeyID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Key")
	FText DisplayName;

};
