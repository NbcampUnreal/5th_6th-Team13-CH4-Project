#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCChatting_Lobby.generated.h"

class UTextBlock;
class UEditableText;
class UScrollBox;

DECLARE_DELEGATE_OneParam(FOnChatCommitted, const FString&);

UCLASS()
class FABRICATION_API UFCChatting_Lobby : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void SetCurrentRoomName(const FText& InRoomName);
	void AddSystemMessage(const FText& Message);
	void AddChatMessage(const FText& Message);
	bool HandleEnterKey();

	FOnChatCommitted OnChatCommitted;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> ChatText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
private:
	void ActivateChatText();
	void DeactivateChatText();
};
