#include "GameMode/FCGameMode_Lobby.h"
#include "GameState/FCGameState_Lobby.h"
#include "Controller/FCPlayerController_Lobby.h"
#include "PlayerState/FCPlayerState_Lobby.h"
#include "GameInstance/FCGameInstance.h"

AFCGameMode_Lobby::AFCGameMode_Lobby()
	: TravelDelayAfterAllReady(5)
	, MinimumPlayerNum(2)
{
	GameStateClass = AFCGameState_Lobby::StaticClass();
	PlayerControllerClass = AFCPlayerController_Lobby::StaticClass();
	PlayerStateClass = AFCPlayerState_Lobby::StaticClass();
}

void AFCGameMode_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllers.Add(NewPlayer);
	
	// 새 플레이어 입장 시 준비 상태 재확인
	AFCGameState_Lobby* GS = GetGameState<AFCGameState_Lobby>();
	if (IsValid(GS))
	{
		GS->CheckAllPlayersReady();
	}
	
	// 플레이어 접속 후 게임 시작 체크
	CheckAndStartGameTravel();
}

void AFCGameMode_Lobby::Logout(AController* Exiting)
{	
	Super::Logout(Exiting);
	
	// 기존 타이머들 모두 정리
	GetWorldTimerManager().ClearTimer(TravelToGameMapTimerHandle);
	GetWorldTimerManager().ClearTimer(TravelDelayCountdownTimerHandle);
	
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
	
	// 준비 상태 재확인 (새 플레이어 입장 등 상황 대비)
	GS->CheckAllPlayersReady();
	
	// 카운트다운이 진행 중이었는지 확인 (타이머 정리 전에 확인)
	bool bWasCountdownActive = GetWorldTimerManager().IsTimerActive(TravelDelayCountdownTimerHandle);
	
	GetWorldTimerManager().ClearTimer(TravelToGameMapTimerHandle);
	GetWorldTimerManager().ClearTimer(TravelDelayCountdownTimerHandle);
	
	// 모든 플레이어가 준비되었고, 최소 인원 이상이면 타이머 시작
	if (GS->bAllPlayersReady && GS->PlayerArray.Num() >= MinimumPlayerNum)
	{
		UE_LOG(LogTemp, Warning, TEXT("모든 플레이어 준비 완료! %d초 후 게임 맵으로 이동합니다."), TravelDelayAfterAllReady);
		
		RemainingTravelDelay = TravelDelayAfterAllReady;
		
		FString CountdownMessage = FString::Printf(TEXT("게임 시작까지 %d초 남았습니다."), RemainingTravelDelay);
		SendChatMessage(CountdownMessage, EMessageType::System);
		
		GetWorldTimerManager().SetTimer(
			TravelDelayCountdownTimerHandle,
			this,
			&AFCGameMode_Lobby::UpdateTravelDelayCountdown,
			1.0f,
			true
		);
	}
	else
	{
		// 카운트다운이 진행 중이었을 때만 취소 메시지 전송
		if (bWasCountdownActive)
		{
			UE_LOG(LogTemp, Warning, TEXT("게임 시작이 취소되었습니다."));
			FString CancelMessage = FString::Printf(TEXT("게임 시작이 취소되었습니다."));
			SendChatMessage(CancelMessage, EMessageType::System);
		}
	}
}

void AFCGameMode_Lobby::UpdateTravelDelayCountdown()
{
	if (!HasAuthority()) return;
	
	// 남은 시간 감소
	RemainingTravelDelay--;
	
	if (RemainingTravelDelay > 0)
	{
		// 시스템 메시지 전송
		FString CountdownMessage = FString::Printf(TEXT("게임 시작까지 %d초 남았습니다."), RemainingTravelDelay);
		SendChatMessage(CountdownMessage, EMessageType::System);
	}
	else
	{
		// 시간이 다 되었으므로 게임 맵으로 이동
		GetWorldTimerManager().ClearTimer(TravelDelayCountdownTimerHandle);
		TravelToGameMap();
	}
}