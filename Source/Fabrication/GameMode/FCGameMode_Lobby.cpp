#include "GameMode/FCGameMode_Lobby.h"
#include "GameState/FCGameState_Lobby.h"
#include "Controller/FCPlayerController_Lobby.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "GameInstance/FCGameInstance.h"

AFCGameMode_Lobby::AFCGameMode_Lobby()
	: GameModeRoomID(1)
{
	GameStateClass = AFCGameState_Lobby::StaticClass();
	PlayerControllerClass = AFCPlayerController_Lobby::StaticClass();
	PlayerStateClass = AFCPlayerState_Lobby::StaticClass();
}

void AFCGameMode_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllers.Add(NewPlayer);
	
	// 첫 번째 플레이어를 방장으로 설정
	AFCGameState_Lobby* GS = GetGameState<AFCGameState_Lobby>();
	if (IsValid(GS) && !IsValid(GS->RoomHost))
	{
		GS->SetRoomHost(NewPlayer);
	}
	
	// 클라이언트에서 닉네임 설정을 기다린 후, 서버에서도 GameInstance에서 닉네임을 가져와서 설정
	// (클라이언트의 BeginPlay가 늦게 호출될 수 있으므로)
	if (AFCPlayerController_Lobby* LobbyPC = Cast<AFCPlayerController_Lobby>(NewPlayer))
	{
		// 서버에서도 GameInstance를 통해 닉네임을 가져올 수 있지만,
		// 일반적으로는 클라이언트에서 ServerRPC를 통해 설정하는 것이 맞습니다.
		// 여기서는 방장 설정만 처리합니다.
	}
	
	// 플레이어 접속 후 게임 시작 체크
	CheckAndStartGameTravel();
}

void AFCGameMode_Lobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	GetWorldTimerManager().ClearTimer(TravelToGameMapTimerHandle);
	
	CheckAndStartGameTravel();
}

void AFCGameMode_Lobby::SendChatMessage(const FString& Message, EMessageType Type)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AFCPlayerController_Lobby* PC_Lobby = Cast<AFCPlayerController_Lobby>(*It);
		if (IsValid(PC_Lobby))
		{
			PC_Lobby->ClientRPCAddChatMessage(Message, Type);
		}
	}
}

void AFCGameMode_Lobby::TravelToGameMap()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("로비 맵에서 게임 맵으로 이동: %s"), *GameMapPath);
		GetWorld()->ServerTravel(GameMapPath + TEXT("?listen"));
	}
}

void AFCGameMode_Lobby::CheckAndStartGameTravel()
{
	if (!HasAuthority()) return;
	
	AFCGameState_Lobby* GS = GetGameState<AFCGameState_Lobby>();
	if (!IsValid(GS)) return;
	
	GetWorldTimerManager().ClearTimer(TravelToGameMapTimerHandle);
	
	// 모든 플레이어가 준비되었고, 최소 2명 이상이면 타이머 시작
	if (GS->bAllPlayersReady && GS->PlayerArray.Num() >= 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("모든 플레이어 준비 완료! %.1f초 후 게임 맵으로 이동합니다."), TravelDelayAfterAllReady);
		
		GetWorldTimerManager().SetTimer(
			TravelToGameMapTimerHandle,
			this,
			&AFCGameMode_Lobby::TravelToGameMap,
			TravelDelayAfterAllReady,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("게임 시작이 취소되었습니다."));
	}
}

#pragma region Room

int32 AFCGameMode_Lobby::CreateRoom(const FText& InRoomName, int32 MaxPlayers)
{
	AFCGameState_Lobby* GS = GetGameState<AFCGameState_Lobby>();
	if (!IsValid(GS)) return INDEX_NONE;

	const int32 NewRoomID = GameModeRoomID++;

	FRoomInfo NewRoom;
	NewRoom.RoomID = NewRoomID;
	NewRoom.RoomName = InRoomName;
	NewRoom.MaxPlayerCount = MaxPlayers;
	NewRoom.CurrentPlayerCount = 0;

	GS->RoomList.Add(NewRoom);

	return NewRoomID;
}

bool AFCGameMode_Lobby::JoinRoom(APlayerController* InPlayer, int32 InRoomID)
{
	if (!IsValid(InPlayer)) return false;

	AFCPlayerState_Lobby* PS = InPlayer->GetPlayerState<AFCPlayerState_Lobby>();
	if (!IsValid(PS)) return false;

	AFCGameState_Lobby* GS = GetGameState<AFCGameState_Lobby>();
	if (!IsValid(GS)) return false;

	// 방 찾기
	FRoomInfo* TargetRoom = GS->RoomList.FindByPredicate(
		[InRoomID](const FRoomInfo& Info) 
		{ 
			return Info.RoomID == InRoomID; 
		}
	);

	if (!TargetRoom) return false; // 방 없음

	if (TargetRoom->CurrentPlayerCount >= TargetRoom->MaxPlayerCount)
	{
		return false; // 정원 초과
	}

	if (PS->GetCurrentRoomID() != INDEX_NONE)
	{
		LeaveRoom(InPlayer);
	}

	PS->SetCurrentRoomID(InRoomID);
	TargetRoom->CurrentPlayerCount++;

	return true;
}

void AFCGameMode_Lobby::LeaveRoom(APlayerController* InPlayer)
{
	if (!IsValid(InPlayer)) return;

	AFCPlayerState_Lobby* PS = InPlayer->GetPlayerState<AFCPlayerState_Lobby>();
	if (!IsValid(PS)) return;

	AFCGameState_Lobby* GS = GetGameState<AFCGameState_Lobby>();
	if (!IsValid(GS)) return;

	const int32 CurrentRoomID = PS->GetCurrentRoomID();
	if (CurrentRoomID == INDEX_NONE) return;

	FRoomInfo* RoomInfo = GS->RoomList.FindByPredicate(
		[CurrentRoomID](const FRoomInfo& Info)
		{
			return Info.RoomID == CurrentRoomID;
		}
	);

	if (RoomInfo)
	{
		RoomInfo->CurrentPlayerCount = FMath::Max(0, RoomInfo->CurrentPlayerCount - 1);

		if (RoomInfo->CurrentPlayerCount <= 0)
		{
			RemoveRoom(CurrentRoomID);
		}
	}

	PS->SetCurrentRoomID(INDEX_NONE);
}

void AFCGameMode_Lobby::RemoveRoom(int32 InRoomID)
{
	AFCGameState_Lobby* GS = GetGameState<AFCGameState_Lobby>();
	if (!IsValid(GS)) return;

	GS->RoomList.RemoveAll([InRoomID](const FRoomInfo& Info)
	{
		return Info.RoomID == InRoomID;
	});
}

#pragma endregion