#include "PlayerState/FCPlayerState.h"

#include "Animation/FCAnimInstance.h"
#include "Components/WidgetComponent.h"
#include "Controller/FCPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Player/Components//UI/FC_PlayerHealth.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/NickNameWidget.h"

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
	//UE_LOG(LogTemp, Warning, TEXT("ChangedPlayerNickName: %s"), *PlayerNickName);
	
	APawn* Pawn = GetPawn();
	if (!Pawn) return;

	if (UWidgetComponent* WC = Pawn->FindComponentByClass<UWidgetComponent>())
	{
		if (UNickNameWidget* Widget = Cast<UNickNameWidget>(WC->GetUserWidgetObject()))
		{
			Widget->SetPlayerNickName(PlayerNickName);
		}
	}
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
		FCPlayerCharacter->HideAllInteractWidgets();
		FCPlayerCharacter->PlayMontage(EMontage::Die);
	}
	
}

void AFCPlayerState::SetPlayerNickName(const FString& NewNickName)
{
	PlayerNickName = NewNickName;
	UE_LOG(LogTemp, Warning, TEXT("PlayerNickName: %s"), *PlayerNickName);
}

