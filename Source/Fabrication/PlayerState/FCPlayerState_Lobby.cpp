#include "PlayerState/FCPlayerState_Lobby.h"
#include "Controller/FCPlayerController_Lobby.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AFCPlayerState_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, PlayerNickName);

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
