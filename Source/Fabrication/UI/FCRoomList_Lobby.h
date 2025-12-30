#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCRoomList_Lobby.generated.h"

class UTextBlock;
class UScrollBox;

//DECLARE_DELEGATE_OneParam(FOnRoomSelected, const FString&);

UCLASS()
class FABRICATION_API UFCRoomList_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPlayerNickNameText(const FText& InNickName);

	//FOnRoomSelected OnRoomSelected;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ViewPlayerNickName;
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UScrollBox> RoomListScroll;
};
