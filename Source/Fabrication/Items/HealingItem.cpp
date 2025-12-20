#include "Items/HealingItem.h"
#include "Components/BoxComponent.h"

AHealingItem::AHealingItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	ItemID = TEXT("HealingItem"); // 임시값

	//Drop Test용 
	//StaticMeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	//StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//StaticMeshComp->SetSimulatePhysics(true);
	//StaticMeshComp->SetEnableGravity(true);

	//BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
	if (IsValid(BoxComp))
	{
		StaticMeshComp->SetCollisionProfileName("OverlapAll");
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AHealingItem::SetVisbilityHealItem(bool bIsShow)
{
	UE_LOG(LogTemp, Warning, TEXT("[Inventory] SetVisbilityHealItem Potion"));
	SetActorHiddenInGame(bIsShow);
}
