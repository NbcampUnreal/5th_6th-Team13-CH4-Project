#include "GameMode/FCGameMode_Lobby.h"
#include "GameState/FCGameState_Lobby.h"
#include "Controller/FCPlayerController_Lobby.h"
#include "PlayerState/FCPlayerState_Lobby.h"

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

}

void AFCGameMode_Lobby::SendChatMessage(const FString& Message, EMessageType Type)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AFCPlayerController_Lobby* PC_Lobby = Cast<AFCPlayerController_Lobby>(*It);
		if (IsValid(PC_Lobby))
		{
			PC_Lobby->ClientRPCAddChatMessage(Message);
		}
	}
}

// 로비에서 자동으로 게임맵으로 이동하는 테스트용 로직
void AFCGameMode_Lobby::TravelToGameMap()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("로비 맵에서 게임 맵으로 이동: %s"), *GameMapPath);
		GetWorld()->ServerTravel(GameMapPath + TEXT("?listen"));
	}
}

void AFCGameMode_Lobby::BeginPlay()
{
	Super::BeginPlay();
	
	// 테스트용: AutoTravelDelay가 0보다 크면 자동으로 게임 맵으로 이동
	if (AutoTravelDelay > 0.0f && HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			AutoTravelTimerHandle,
			this,
			&AFCGameMode_Lobby::TravelToGameMap,
			AutoTravelDelay,
			false
		);
		UE_LOG(LogTemp, Warning, TEXT("로비 맵: %.1f초 후 자동으로 게임 맵으로 이동합니다."), AutoTravelDelay);
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