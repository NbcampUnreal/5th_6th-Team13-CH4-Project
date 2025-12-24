#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCRoomList_Lobby.generated.h"

class UTextBlock;
class UScrollBox;
class UButton;

DECLARE_DELEGATE_OneParam(FOnRoomSelected, const FString&);

UCLASS()
class FABRICATION_API UFCRoomList_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	void SetPlayerNickNameText(const FText& InNickName);
	void AddRoomEntry(const FText& RoomTitle);

	FOnRoomSelected OnRoomSelected;
	
	// 세션 생성 테스트 버튼 클릭 함수
	UFUNCTION()
	void OnCreateSessionButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ViewPlayerNickName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> RoomListScroll;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateSessionButton;
};
