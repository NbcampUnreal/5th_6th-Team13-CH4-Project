#include "PlayerState/FCPlayerState.h"

#include "Animation/FCAnimInstance.h"
#include "Controller/FCPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"

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
	if (AFCPlayerCharacter* FCPlayerCharacter = GetPawn<AFCPlayerCharacter>())
	{
		if (UAnimInstance* AnimInstance = Cast<UAnimInstance>(FCPlayerCharacter->GetMesh()->GetAnimInstance()))
		{
			if (UFCAnimInstance* FCAI = Cast<UFCAnimInstance>(AnimInstance))
			{
				FCAI->bIsDead = bIsDead;
				if (bIsDead)
				{
					FCPlayerCharacter->PlayMontage(EMontage::Die);
				}
			}
		}
	}
}

void AFCPlayerState::SetPlayerNickName(const FString& NewNickName)
{
	PlayerNickName = NewNickName;
	UE_LOG(LogTemp, Warning, TEXT("PlayerNickName: %s"), *PlayerNickName);
}

