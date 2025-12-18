#include "Controller/FCPlayerController_Lobby.h"
#include "GameInstance/FCGameInstance.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "GameMode/FCGameMode_Lobby.h"
#include "UI/FCChatting_Lobby.h"
#include "UI/FCChatMessage_Lobby.h"
#include "UI/FCRoomList_Lobby.h"
#include "UI/FCRoomWaiting_Lobby.h"
#include "Components/ScrollBox.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AFCPlayerController_Lobby::AFCPlayerController_Lobby()
	: LobbyInputMapping(nullptr)
	, IA_OpenChat(nullptr)
	, ChatWidgetClass(nullptr)
	, ChatWidgetInstance(nullptr)
	, ChatMessageClass(nullptr)
	, ChatMessageInstance(nullptr)
	, RoomListWidgetClass(nullptr)
	, RoomListWidgetInstance(nullptr)
	, RoomWaitingWidgetClass(nullptr)
	, RoomWaitingWidgetInstance(nullptr)
{
}

void AFCPlayerController_Lobby::ActivateChatBox()
{
	UFCChatting_Lobby* ChattingUI = Cast<UFCChatting_Lobby>(ChatWidgetInstance);
	if (!IsValid(ChattingUI)) return;

	ChattingUI->ActivateChatText();
}

void AFCPlayerController_Lobby::ServerRPCSetPlayerNickName_Implementation(const FString& InNickName)
{
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (IsValid(PS))
	{
		PS->SetPlayerNickName(InNickName);
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

void AFCPlayerController_Lobby::ClientRPCAddChatMessage_Implementation(const FString& Message)
{
	AddChatMessage(Message);
}

void AFCPlayerController_Lobby::UpdateNickNameUI(const FString& InNickName)
{
	if (IsValid(RoomListWidgetInstance))
	{
		UFCRoomList_Lobby* RoomListUI = Cast<UFCRoomList_Lobby>(RoomListWidgetInstance);
		if (IsValid(RoomListUI))
		{
			RoomListUI->SetPlayerNickNameText(InNickName);
		}
	}
}

void AFCPlayerController_Lobby::AddChatMessage(const FString& Message)
{
	if (!IsValid(ChatWidgetInstance)) return;

	UFCChatting_Lobby* ChattingUI = Cast<UFCChatting_Lobby>(ChatWidgetInstance);
	if (IsValid(ChattingUI))
	{
		if (IsValid(ChatMessageClass))
		{
			ChatMessageInstance = CreateWidget<UUserWidget>(this, ChatMessageClass);
			UFCChatMessage_Lobby* MessageUI = Cast<UFCChatMessage_Lobby>(ChatMessageInstance);
			if (IsValid(MessageUI))
			{
				MessageUI->SetChatMessage(Message);
				ChattingUI->ChatScrollBox->AddChild(ChatMessageInstance);
				ChattingUI->ChatScrollBox->ScrollToEnd();
				ChattingUI->ChatScrollBox->bAnimateWheelScrolling = true;
			}
		}
	}
}

void AFCPlayerController_Lobby::OnNickNameUpdated()
{
	AFCPlayerState_Lobby* PS = GetPlayerState<AFCPlayerState_Lobby>();
	if (!IsValid(PS)) return;

	UpdateNickNameUI(PS->GetPlayerNickName());
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

		UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (IsValid(Subsystem))
		{
			Subsystem->AddMappingContext(LobbyInputMapping, 0);
		}

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);

		bShowMouseCursor = true;
	}
	
	if (IsValid(ChatWidgetClass))
	{
		ChatWidgetInstance = CreateWidget<UUserWidget>(this, ChatWidgetClass);
		
		if (IsValid(ChatWidgetInstance))
		{
			ChatWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(RoomListWidgetClass))
	{
		RoomListWidgetInstance = CreateWidget<UUserWidget>(this, RoomListWidgetClass);

		if (IsValid(RoomListWidgetInstance))
		{
			RoomListWidgetInstance->AddToViewport();
			OnNickNameUpdated();
		}
	}

	if (IsValid(RoomWaitingWidgetClass))
	{
		RoomWaitingWidgetInstance = CreateWidget<UUserWidget>(this, RoomWaitingWidgetClass);
		if (IsValid(RoomWaitingWidgetInstance))
		{
			RoomWaitingWidgetInstance->AddToViewport();
		}
	}

}

void AFCPlayerController_Lobby::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIComp->BindAction(
			IA_OpenChat, 
			ETriggerEvent::Started, 
			this, 
			&AFCPlayerController_Lobby::ChatButtonPressed
		);
	}
}

void AFCPlayerController_Lobby::ChatButtonPressed(const FInputActionValue& Value)
{
	if (!IsValid(ChatWidgetInstance)) return;

	ActivateChatBox();
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
	}
}

#pragma endregion