#include "Controller/FCPlayerController_Lobby.h"
#include "GameInstance/FCGameInstance.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "GameMode/FCGameMode_Lobby.h"
#include "GameState/FCGameState_Lobby.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UI/FCHUD_Lobby.h"
#include "UI/FCChatting_Lobby.h"
#include "UI/FCRoomList_Lobby.h"
#include "UI/FCRoomWaiting_Lobby.h"

AFCPlayerController_Lobby::AFCPlayerController_Lobby()
	: HUD_LobbyClass(nullptr)
	, HUD_Lobby(nullptr)
{
}

void AFCPlayerController_Lobby::ServerRPCSetPlayerNickName_Implementation(const FString& InNickName)
{
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(PS))
	{
		PS->SetPlayerNickName(InNickName);
		if (AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>())
		{
			FString LoginMessage = FString::Printf(TEXT("%s 님이 입장하셨습니다."), *InNickName);
			GM->SendChatMessage(LoginMessage, EMessageType::System);
		}
	}
}

void AFCPlayerController_Lobby::ServerRPCSendChatMessage_Implementation(const FString& Message)
{
	AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>();
	if (IsValid(GM))
	{
		GM->SendChatMessage(Message);
	}
}

void AFCPlayerController_Lobby::ClientRPCAddChatMessage_Implementation(const FString& Message, EMessageType Type)
{
	AddChatMessage(Message, Type);
}

void AFCPlayerController_Lobby::UpdateNickNameUI(const FString& InNickName)
{
	if (IsValid(HUD_Lobby))
	{
		UFCRoomList_Lobby* RoomList = HUD_Lobby->GetWidget<UFCRoomList_Lobby>();
		if (!IsValid(RoomList)) return;

		FText DisplayNickName = FText::FromString(InNickName);
		RoomList->SetPlayerNickNameText(DisplayNickName);
	}
}

void AFCPlayerController_Lobby::SetChatMessage(const FString& Message)
{
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (!IsValid(PS)) return;

	FString PlayerMessage = FString::Printf(TEXT("%s : %s"), *PS->GetPlayerNickName(), *Message);

	ServerRPCSendChatMessage(PlayerMessage);
}

void AFCPlayerController_Lobby::AddChatMessage(const FString& Message, EMessageType Type)
{
	if (IsValid(HUD_Lobby))
	{
		UFCChatting_Lobby* Chatting = HUD_Lobby->GetWidget<UFCChatting_Lobby>();
		if (!IsValid(Chatting)) return;

		FText DisplayMessage = FText::FromString(Message);

		switch (Type)
		{
		case EMessageType::Default:
			Chatting->AddChatMessage(DisplayMessage);
			break;
		case EMessageType::System:
			Chatting->AddSystemMessage(DisplayMessage);
			break;
		default:
			break;
		}	
	}

}

void AFCPlayerController_Lobby::OnNickNameUpdated()
{
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (!IsValid(PS)) return;

	UpdateNickNameUI(PS->GetPlayerNickName());
	UpdatePlayerListUI();
}

void AFCPlayerController_Lobby::UpdatePlayerListUI()
{
	if (!IsLocalController())
	{
		return;
	}
	
	AFCGameState_Lobby* GS = GetWorld()->GetGameState<AFCGameState_Lobby>();
	if (!IsValid(GS))
	{
		return;
	}
	
	if (!IsValid(HUD_Lobby))
	{
		return;
	}
	
	UFCRoomWaiting_Lobby* RoomWaiting = HUD_Lobby->GetWidget<UFCRoomWaiting_Lobby>();
	
	if (!IsValid(RoomWaiting))
	{
		return;
	}
	
	TArray<FText> PlayerNames;
	TArray<bool> PlayerReadyStates;
	
	// 로컬 플레이어의 준비 상태 확인용
	bool bLocalPlayerReady = false;
	
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (AFCPlayerState_Lobby* LobbyPS = Cast<AFCPlayerState_Lobby>(PS))
		{
			FString NickName = LobbyPS->GetPlayerNickName();
			if (NickName.IsEmpty())
			{
				NickName = TEXT("Player");
			}
			
			PlayerNames.Add(FText::FromString(NickName));
			bool bIsReady = LobbyPS->IsReady();
			PlayerReadyStates.Add(bIsReady);
			
			// 로컬 플레이어의 준비 상태 확인
			if (PS == PlayerState)
			{
				bLocalPlayerReady = bIsReady;
			}
		}
	}
	
	if (PlayerNames.Num() > 3)
	{
		PlayerNames.SetNum(3);
		PlayerReadyStates.SetNum(3);
	}
	
	RoomWaiting->UpdatePlayerList(PlayerNames, PlayerReadyStates);
	RoomWaiting->UpdateReadyButtonText(bLocalPlayerReady);
}

void AFCPlayerController_Lobby::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	if (IsLocalPlayerController())
	{
		UFCGameInstance* GI = GetGameInstance<UFCGameInstance>();
		if (IsValid(GI))
		{
			ServerRPCSetPlayerNickName(GI->GetLocalPlayerNickName());
		}
	}
	
	if (IsValid(HUD_LobbyClass))
	{
		HUD_Lobby = CreateWidget<UFCHUD_Lobby>(this, HUD_LobbyClass);
		if (IsValid(HUD_Lobby))
		{
			HUD_Lobby->AddToViewport();
			OnNickNameUpdated();

			UFCChatting_Lobby* Chatting = HUD_Lobby->GetWidget<UFCChatting_Lobby>();
			if (IsValid(Chatting))
			{
				Chatting->OnChatCommitted.BindUObject(
					this,
					&AFCPlayerController_Lobby::SetChatMessage
				);
			}
			
			UFCRoomWaiting_Lobby* RoomWaiting = HUD_Lobby->GetWidget<UFCRoomWaiting_Lobby>();
			if (IsValid(RoomWaiting))
			{
				RoomWaiting->OnReadyButtonClicked.BindUObject(
					this,
					&AFCPlayerController_Lobby::ToggleReady
				);
			}

			FInputModeUIOnly InputMode;

			InputMode.SetWidgetToFocus(HUD_Lobby->GetCachedWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);

			bShowMouseCursor = true;
		}
	}
	
	// 초기 플레이어 목록 업데이트
	UpdatePlayerListUI();
	
	// 주기적으로 플레이어 목록 업데이트 (플레이어 접속/퇴장 감지용)
	GetWorldTimerManager().SetTimer(
		PlayerListUpdateTimerHandle,
		this,
		&AFCPlayerController_Lobby::UpdatePlayerListUI,
		0.5f, // 0.5초마다 업데이트
		true  // 반복
	);

}

#pragma region Ready

void AFCPlayerController_Lobby::ToggleReady()
{
	AFCPlayerState_Lobby* FCPS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(FCPS))
	{
		bool bNewReady = !FCPS->bIsReady;
		ServerRPCSetReady(bNewReady);
	}
}

void AFCPlayerController_Lobby::ServerRPCCreateRoom_Implementation(const FText& RoomName, int32 MaxPlayers)
{
	// 1. 서버 권한 확인
	if (!HasAuthority())
	{
		return;
	}
    
	// 2. GameMode 가져오기
	AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>();
	if (!IsValid(GM))
	{
		ClientRPCOnRoomCreated(INDEX_NONE, false, "게임 모드를 찾을 수 없습니다.");
		return;
	}
    
	// 3. 이미 방에 있는지 확인
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(PS) && PS->GetCurrentRoomID() != INDEX_NONE)
	{
		ClientRPCOnRoomCreated(INDEX_NONE, false, "이미 방에 참가 중입니다.");
		return;
	}
    
	// 4. GameMode의 CreateRoom 호출
	int32 RoomID = GM->CreateRoom(RoomName, MaxPlayers);
    
	// 5. 생성 실패 시 클라이언트에 알림
	if (RoomID == INDEX_NONE)
	{
		ClientRPCOnRoomCreated(INDEX_NONE, false, "방 생성에 실패했습니다.");
		return;
	}
    
	// 6. 생성 성공 시 자동으로 입장
	bool bJoined = GM->JoinRoom(this, RoomID);
	if (bJoined) 
	{
		ClientRPCOnRoomCreated(RoomID, true);
	}
	else
	{
		ClientRPCOnRoomCreated(INDEX_NONE, false, "방 생성 후 입장에 실패했습니다.");
	}	
}

void AFCPlayerController_Lobby::ServerRPCJoinRoom_Implementation(int32 RoomID)
{
	// 1. 서버 권한 확인
	if (!HasAuthority())
	{
		return;
	}
    
	// 2. GameMode 가져오기
	AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>();
	if (!IsValid(GM))
	{
		ClientRPCOnRoomJoined(INDEX_NONE, false, "게임 모드를 찾을 수 없습니다.");
		return;
	}
    
	// 3. 이미 다른 방에 있으면 먼저 나가기
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(PS) && PS->GetCurrentRoomID() != INDEX_NONE && PS->GetCurrentRoomID() != RoomID)
	{
		GM->LeaveRoom(this);
	}
    
	// 4. 방 입장 시도
	bool bSuccess = GM->JoinRoom(this, RoomID);
    
	if (bSuccess)
	{
		ClientRPCOnRoomJoined(RoomID, true);
	}
	else
	{
		// 5. 실패 원인 파악하여 에러 메시지 전달
		AFCGameState_Lobby* GS = GetWorld()->GetGameState<AFCGameState_Lobby>();
		FString ErrorMsg = "방 입장에 실패했습니다.";
        
		if (IsValid(GS))
		{
			FRoomInfo* RoomInfo = GS->RoomList.FindByPredicate([RoomID](const FRoomInfo& Info)
			{ 
				return Info.RoomID == RoomID; 
			});
            
			if (!RoomInfo)
			{
				ErrorMsg = "방을 찾을 수 없습니다.";
			} else if (RoomInfo->CurrentPlayerCount >= RoomInfo->MaxPlayerCount)
			{
				ErrorMsg = "방이 가득 찼습니다.";
			}
		}
        
		ClientRPCOnRoomJoined(INDEX_NONE, false, ErrorMsg);
	}
}

void AFCPlayerController_Lobby::ServerRPCLeaveRoom_Implementation()
{
	// 1. 서버 권한 확인
	if (!HasAuthority())
	{
		return;
	}
    
	// 2. GameMode 가져오기
	AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>();
	if (!IsValid(GM))
	{
		return;
	}
    
	// 3. 현재 방에 있는지 확인
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (!IsValid(PS) || PS->GetCurrentRoomID() == INDEX_NONE)
	{
		return;
	}
    
	// 4. GameMode의 LeaveRoom 호출
	GM->LeaveRoom(this);
    
	// 5. 클라이언트에 알림
	ClientRPCOnRoomLeft();
}

void AFCPlayerController_Lobby::ClientRPCOnRoomCreated_Implementation(int32 RoomID, bool bSuccess,
	const FString& ErrorMessage)
{
	if (bSuccess)
	{
		// UI에서 방 대기실로 전환
		UpdatePlayerListUI(); // 방 내 플레이어 목록 업데이트
	}
	else
	{
		// UI에 에러 메시지 표시
	}
}

void AFCPlayerController_Lobby::ClientRPCOnRoomJoined_Implementation(int32 RoomID, bool bSuccess,
	const FString& ErrorMessage)
{
	// UI에서 방 목록으로 전환
}

void AFCPlayerController_Lobby::ClientRPCOnRoomLeft_Implementation()
{
	// UI에서 방 목록으로 전환
}

void AFCPlayerController_Lobby::ClientRPCUpdateRoomList_Implementation(const TArray<FRoomInfo>& RoomList)
{
	// 일단 로그로 확인
	UE_LOG(LogTemp, Log, TEXT("방 목록 업데이트: %d개 방"), RoomList.Num());

	// TODO: UI 위젯에 방 목록 업데이트 함수가 구현되면 호출
	// if (IsValid(HUD_Lobby))
	// {
	//     UFCRoomList_Lobby* RoomListWidget = HUD_Lobby->GetWidget<UFCRoomList_Lobby>();
	//     if (IsValid(RoomListWidget))
	//     {
	//         RoomListWidget->UpdateRoomList(RoomList); // 나중에 구현할 함수
	//     }
	// }
}

void AFCPlayerController_Lobby::ServerRPCSetReady_Implementation(bool bReady)
{
	AFCPlayerState_Lobby* FCPS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(FCPS))
	{
		FCPS->bIsReady = bReady;
		
		// 모든 플레이어 준비 상태 체크
		AFCGameState_Lobby* GS = GetWorld()->GetGameState<AFCGameState_Lobby>();
		if (IsValid(GS))
		{
			GS->CheckAllPlayersReady();
			
			// GameMode에서 게임 맵 이동 체크
			AFCGameMode_Lobby* GM = GetWorld()->GetAuthGameMode<AFCGameMode_Lobby>();
			if (IsValid(GM))
			{
				GM->CheckAndStartGameTravel();
			}
		}
	}
}

#pragma endregion