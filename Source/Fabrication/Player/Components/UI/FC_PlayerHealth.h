#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "FC_PlayerHealth.generated.h"

UCLASS()
class FABRICATION_API UFC_PlayerHealth : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override; 

public:
#pragma region Candle Setting 
	UPROPERTY(meta=(BindWidget))
	UImage* Image_Health_1;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Health_2;

	UPROPERTY(meta=(BindWidget))
	UImage* Image_Health_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	UTexture2D* CandleOn; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	UTexture2D* CandleOff;
#pragma endregion 

#pragma region Smoke Setting 
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Smoke_1;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Smoke_2;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Smoke_3;
#pragma endregion 

#pragma region Animation 
	//±ôºýÀÓ 
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Candle_1_Slow;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Candle_2_Slow;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Candle_3_Slow;

	//¿¬±â 
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Smoke_1;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Smoke_2;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Smoke_3;
#pragma endregion 

public:
	UFUNCTION(BlueprintCallable, Category="Health")
	void UpdateHealth();

private:
	void PlayFlickerAnimation(int32 FlickerIndex);
	void PlaySmokeAnimation(int32 SmokeIndex);

	int32 LastKnownHP = 3;
};
