#include "Player/Components/StatusComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"

UStatusComponent::UStatusComponent() :
	MaxHP(3)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	CurrentHP = MaxHP;
}

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentHP);
}

void UStatusComponent::OnRep_ChangeHP()
{
	if (CurrentHP <= 0)
	{
		if (AFCPlayerCharacter* FCPlayerCharacter = Cast<AFCPlayerCharacter>(GetOwner()))
		{
			FCPlayerCharacter->OnPlayerDiePreProssessing();
		}
	}

	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UStatusComponent::SetCurrentHP(int32 InCurHP)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return;
	}

	CurrentHP = InCurHP;
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UStatusComponent::SetMaxHP(int32 InMaxHP)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}

	MaxHP = InMaxHP;

	if (MaxHP < 0)
	{
		MaxHP = 1;
	}
}

int32 UStatusComponent::ApplyDamage(int32 InDamage)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return 0;
	}

	const int32 PrevHP = CurrentHP;
	const int32 ActualDamage = FMath::Clamp(InDamage, 0, PrevHP);

	SetCurrentHP(PrevHP - ActualDamage);

	return ActualDamage;
}

