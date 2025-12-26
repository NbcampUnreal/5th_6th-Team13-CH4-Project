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