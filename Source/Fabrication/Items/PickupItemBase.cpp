#include "Items/PickupItemBase.h"
#include "Components/BoxComponent.h"
#include "Player/FCPlayerCharacter.h"
#include "Items/Inventory/FC_InventoryComponent.h"

APickupItemBase::APickupItemBase()
	: ItemID(TEXT("PickupItemBase"))
	, SceneComp(nullptr)
	, StaticMeshComp(nullptr)
	, BoxComp(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComp);
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneComp);
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	BoxComp->SetupAttachment(SceneComp);
}
void APickupItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (!BoxComp->OnComponentBeginOverlap.IsAlreadyBound(this, &APickupItemBase::OnItemOverlap))
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &APickupItemBase::OnItemOverlap);
	}
	if (!BoxComp->OnComponentEndOverlap.IsAlreadyBound(this, &APickupItemBase::OnItemEndOverlap))
	{
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &APickupItemBase::OnItemEndOverlap);
	}

}

void APickupItemBase::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("OverlapBegin/name %s"), *ItemID.ToString());
	UE_LOG(LogTemp, Error, TEXT("Name :  %s"), *this->GetName());

	if (HasAuthority())
	{
		if (AFCPlayerCharacter* FCPlayerCharacter = Cast<AFCPlayerCharacter>(OtherActor))
		{
			FCPlayerCharacter->bIsDetectPickUpTrigger = true;
		}

	}
}

void APickupItemBase::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Error, TEXT("OverlapEnd"));
	if (HasAuthority())
	{
		if (AFCPlayerCharacter* FCPlayerCharacter = Cast<AFCPlayerCharacter>(OtherActor))
		{
			FCPlayerCharacter->bIsDetectPickUpTrigger = false;
		}
	}
}

void APickupItemBase::Interact(ACharacter* User, const FHitResult& HitResult)
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
	if (!IsValid(Player)) return;

	Player->InvenComp->AddItem(GetItemID());
	Destroy();
}

FName APickupItemBase::GetItemID() const
{
	return ItemID;
}
