#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCRoomList_Lobby.generated.h"

class UTextBlock;
class UScrollBox;

UCLASS()
class FABRICATION_API UFCRoomList_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPlayerNickNameText(const FString& InNickName);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UTextBlock> ViewPlayerNickName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UScrollBox> RoomListScroll;
};
