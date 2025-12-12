#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCChatMessage_Lobby.generated.h"

class UTextBlock;

UCLASS()
class FABRICATION_API UFCChatMessage_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatMessageTextBlock;

	void SetChatMessage(const FString& Message);
};
