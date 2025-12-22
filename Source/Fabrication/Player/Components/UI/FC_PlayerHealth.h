#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "FC_PlayerHealth.generated.h"

UCLASS()
class FABRICATION_API UFC_PlayerHealth : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	UImage* Image_Health_1;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Health_2;

	UPROPERTY(meta=(BindWidget))
	UImage* Image_Health_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	UTexture2D* CandleOn; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	UTexture2D* CandleLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	UTexture2D* CandleOff; 

public:
	UFUNCTION(BlueprintCallable, Category="Health")
	void UpdateHealth();
	
	virtual void NativeConstruct() override; 
};
