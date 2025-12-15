#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCTitleLayout.generated.h"

class UButton;
class UEditableText;

UCLASS()
class FABRICATION_API UFCTitleLayout : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFCTitleLayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UFUNCTION(BlueprintCallable)
	void FocusNameInput();

protected:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnPlayButtonClicked();
	UFUNCTION()
	void OnExitButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Title", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PlayButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Title", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ExitButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Title", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> ServerIPEditableText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Title", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> PlayerNickNameEditableText;

};
