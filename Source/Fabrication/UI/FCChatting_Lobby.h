#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "FCChatting_Lobby.generated.h"

class UEditableText;
class UScrollBox;

UCLASS()
class FABRICATION_API UFCChatting_Lobby : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ActivateChatText();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> ChatText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;
	
protected:
	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
