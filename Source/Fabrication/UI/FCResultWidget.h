#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCResultWidget.generated.h"

class UTextBlock;
class UImage;
class UTexture2D;

UCLASS()
class FABRICATION_API UFCResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ResultImage;
	
	// 탈출 성공 이미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Result Images")
	TObjectPtr<UTexture2D> EscapeSuccessTexture;
	
	// 탈출 실패 이미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Result Images")
	TObjectPtr<UTexture2D> EscapeFailedTexture;
	
public:	
	void SetResultImage(bool bIsEscaped);
	
	void NativeConstruct() override;
};