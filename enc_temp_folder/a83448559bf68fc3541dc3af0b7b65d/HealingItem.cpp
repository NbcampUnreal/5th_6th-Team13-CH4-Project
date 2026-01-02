#include "Items/HealingItem.h"
#include "Components/BoxComponent.h"
#include "Player/FCPlayerCharacter.h"

AHealingItem::AHealingItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	ItemID = TEXT("HealingItem"); 
}

void AHealingItem::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		AttachSettingHealingItem();
	}
}

void AHealingItem::AttachSettingHealingItem()
{
	if (!IsValid(BoxComp)) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(GetOwner());
	if (Player)
	{
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		StaticMeshComp->SetCollisionProfileName("PickUp");
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	}
}
