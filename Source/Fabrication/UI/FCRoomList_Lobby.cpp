#include "UI/FCRoomList_Lobby.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "GameInstance/FCGameInstance.h"
#include "Engine/GameInstance.h"

void UFCRoomList_Lobby::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 버튼 클릭 이벤트 바인딩
	if (IsValid(CreateSessionButton))
	{
		CreateSessionButton->OnClicked.AddDynamic(this, &UFCRoomList_Lobby::OnCreateSessionButtonClicked);
	}
}

void UFCRoomList_Lobby::SetPlayerNickNameText(const FText& InNickName)
{
	if (IsValid(ViewPlayerNickName))
	{
		ViewPlayerNickName->SetText(InNickName);
	}
}

void UFCRoomList_Lobby::AddRoomEntry(const FText& RoomTitle)
{
	if (!IsValid(RoomListScroll)) return;
	// 생성된 방이름 표시
}

void UFCRoomList_Lobby::OnCreateSessionButtonClicked()
{
	// GameInstance 가져오기
	UFCGameInstance* GameInstance = Cast<UFCGameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		// CreateSession 함수 호출
		GameInstance->CreateSession(TEXT("테스트 세션"), 4);
		UE_LOG(LogTemp, Warning, TEXT("세션 생성 버튼 클릭됨"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance를 가져올 수 없습니다."));
	}
}
