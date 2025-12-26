#include "PlayerState/FCPlayerState.h"

#include "Animation/FCAnimInstance.h"
#include "Controller/FCPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Controller/FCPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

void AFCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bIsReady);
	DOREPLIFETIME(ThisClass, PlayerNickName);
	DOREPLIFETIME(ThisClass, bIsDead);
}

const FString& AFCPlayerState::GetPlayerNickName() const
{
	return PlayerNickName;
}

void AFCPlayerState::OnRep_ChangedPlayerNickName()
{
	UE_LOG(LogTemp, Warning, TEXT("ChangedPlayerNickName: %s"), *PlayerNickName);
}

void AFCPlayerState::OnRep_IsDead()
{
	AFCPlayerCharacter* FCPlayerCharacter = GetPawn<AFCPlayerCharacter>();
	if (!FCPlayerCharacter) return;

	UAnimInstance* AnimInstance = Cast<UAnimInstance>(FCPlayerCharacter->GetMesh()->GetAnimInstance());
	if (!AnimInstance) return;

	UFCAnimInstance* FCAI = Cast<UFCAnimInstance>(AnimInstance);
	if (!FCAI) return;

	FCAI->bIsDead = bIsDead; 
	
	if (bIsDead)
	{
		FCPlayerCharacter->PlayMontage(EMontage::Die);
	}
	
}

void AFCPlayerState::SetPlayerNickName(const FString& NewNickName)
{
	PlayerNickName = NewNickName;
	UE_LOG(LogTemp, Warning, TEXT("PlayerNickName: %s"), *PlayerNickName);
}

