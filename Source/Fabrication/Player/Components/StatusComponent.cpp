#include "Player/Components/StatusComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FCPlayerCharacter.h"
#include "Items/Inventory/FC_InventoryComponent.h"
#include "Controller/FCPlayerController.h"
#include "Player/Components/UI/FC_PlayerHealth.h"

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
	if (AFCPlayerController* PC = Cast<AFCPlayerController>(GetOwner()->GetInstigatorController()))
	{
		PC->HealthWidgetInstance->UpdateHealth();
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
	if (CurrentHP >= 0)
	{
		if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner()))
		{
			Player->OnPlayerDiedProcessing();
			if (Player->InvenComp)
			{
				Player->InvenComp->DropAlIItems();
			}
		}
	}

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

void UStatusComponent::HealHP(int32 Heal)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return;

	if (CurrentHP < MaxHP)
	{
		CurrentHP += Heal;
	}
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

