#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItemBase.h"
#include "RevivalItem.generated.h"

UCLASS()
class FABRICATION_API ARevivalItem : public APickupItemBase
{
	GENERATED_BODY()
	
public:
	ARevivalItem();
	virtual void BeginPlay() override; 

public:
	UFUNCTION()
	void AttachSettingRevivalItem(); 
};
