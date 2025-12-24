#include "Items/RevivalItem.h"
#include "Player/FCPlayerCharacter.h"
#include "Components/BoxComponent.h"

ARevivalItem::ARevivalItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	ItemID = TEXT("RevivalItem");
}

void ARevivalItem::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		AttachSettingRevivalItem();
	}
}

void ARevivalItem::AttachSettingRevivalItem()
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
