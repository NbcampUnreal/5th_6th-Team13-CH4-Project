#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FC_InteractItem.generated.h"

UCLASS()
class FABRICATION_API AFC_InteractItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AFC_InteractItem();

protected:
	virtual void BeginPlay() override;
};
