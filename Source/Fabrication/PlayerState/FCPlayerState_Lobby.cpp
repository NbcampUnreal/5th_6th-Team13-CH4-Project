#include "PlayerState/FCPlayerState_Lobby.h"
#include "Controller/FCPlayerController_Lobby.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AFCPlayerState_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, PlayerNickName);
	DOREPLIFETIME(ThisClass, CurrentRoomID);
	DOREPLIFETIME(ThisClass, bIsReady);
}

void AFCPlayerState_Lobby::SetPlayerNickName(const FString& InNickName)
{
	if (!HasAuthority()) return;

	PlayerNickName = InNickName;
}

const FString& AFCPlayerState_Lobby::GetPlayerNickName() const
{
	return PlayerNickName;
}

void AFCPlayerState_Lobby::OnRep_PlayerNickName()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!IsValid(PC) || !PC->IsLocalController()) return;

	AFCPlayerController_Lobby* LobbyPC = Cast<AFCPlayerController_Lobby>(PC);
	if (!IsValid(LobbyPC)) return;

	LobbyPC->OnNickNameUpdated();
}

void AFCPlayerState_Lobby::OnRep_RoomID()
{
}

int32 AFCPlayerState_Lobby::GetCurrentRoomID() const
{
	return CurrentRoomID;
}

void AFCPlayerState_Lobby::SetCurrentRoomID(int32 RoomID)
{
	CurrentRoomID = RoomID;
}

#pragma region Ready

void AFCPlayerState_Lobby::SetReady(bool bReady)
{
	if (HasAuthority())
	{
		bIsReady = bReady;
	}
}

bool AFCPlayerState_Lobby::IsReady() const
{
	return bIsReady;
}

void AFCPlayerState_Lobby::OnRep_bIsReady()
{
	if (UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (PC->IsLocalController())
				{
					if (AFCPlayerController_Lobby* LobbyPC = Cast<AFCPlayerController_Lobby>(PC))
					{
						LobbyPC->UpdatePlayerListUI();
					}
					break; // 로컬 컨트롤러는 하나만 있으므로
				}
			}
		}
	}
}

#pragma endregion