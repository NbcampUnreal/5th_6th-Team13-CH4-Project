#include "Items/FC_BaseItem.h"
#include "Components/SphereComponent.h"
#include "Inventory/FC_InventoryComponent.h"

AFC_BaseItem::AFC_BaseItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ItemID = "DefaultItem";
	ItemCollisionRadius = 200.0f;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetSphereRadius(ItemCollisionRadius);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AFC_BaseItem::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AFC_BaseItem::OnItemEndOverlap);

	bReplicates = true;

}

void AFC_BaseItem::BeginPlay()
{
	Super::BeginPlay();
}

void AFC_BaseItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ActivateItem(OtherActor);
}

void AFC_BaseItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AFC_BaseItem::ActivateItem(AActor* Activator)
{
	if (!HasAuthority() || !Activator)
		return;

	/*AFCCharacter* Character = Cast<AFCCharacter>(Activator);
	if (!Character) return;*/

	//if (UFC_InventoryComponent* Inv = AFCCharacter->FindComponentByClass<UFC_InventoryComponent>())
	//{
	//	/*const bool IsAddItem = Inv->AddItem(ItemID, 1);*/
	//	/*if (bAdded)
	//	{
	//		DestroyItem();
	//	}*/
	//}
}

void AFC_BaseItem::DestroyItem()
{
	Destroy();
}

FName AFC_BaseItem::GetItemID() const
{
	return ItemID;
}

