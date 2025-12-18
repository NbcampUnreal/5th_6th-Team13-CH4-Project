#pragma once

#include "CoreMinimal.h"
#include "Objects/InteratableObjectBase.h"
#include "FCKeyItem.generated.h"

class UFCKeyDataAsset;
enum class EFCKeyType : uint8;

UCLASS()
class FABRICATION_API AFCKeyItem : public AInteratableObjectBase
{
	GENERATED_BODY()
	
public:
	AFCKeyItem();
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;

	//void InitializeKey(const UFCKeyDataAsset* InKeyData);


private:
	//UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Key")
	//TObjectPtr<const UFCKeyDataAsset> KeyData;
	EFCKeyType KeyID;
	FText DisplayName;
};
