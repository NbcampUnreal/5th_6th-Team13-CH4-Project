#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

class UImage;

UCLASS()
class FABRICATION_API UInteractWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetImage(UTexture2D* InteractImage);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> InteractType;
};
