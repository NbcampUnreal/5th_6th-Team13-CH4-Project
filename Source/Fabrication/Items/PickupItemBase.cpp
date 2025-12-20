#include "Items/PickupItemBase.h"
#include "Components/BoxComponent.h"
#include "Player/FCPlayerCharacter.h"
#include "Items/Inventory/FC_InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Fabrication.h"

APickupItemBase::APickupItemBase()
	: ItemID(TEXT("PickupItemBase"))
	, bIsCollected(false)
	, SceneComp(nullptr)
	, StaticMeshComp(nullptr)
	, BoxComp(nullptr)
	, InteractableWidget(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneComp);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	BoxComp->SetupAttachment(SceneComp);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableUI"));
	InteractableWidget->SetupAttachment(SceneComp);
	InteractableWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractableWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractableWidget->SetCollisionResponseToAllChannels(ECR_Ignore);
	
}

void APickupItemBase::BeginPlay()
{
	Super::BeginPlay();

	InteractableWidget->SetVisibility(false);

	if (!BoxComp->OnComponentBeginOverlap.IsAlreadyBound(this, &APickupItemBase::OnItemOverlap))
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &APickupItemBase::OnItemOverlap);
	}
	if (!BoxComp->OnComponentEndOverlap.IsAlreadyBound(this, &APickupItemBase::OnItemEndOverlap))
	{
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &APickupItemBase::OnItemEndOverlap);
	}

}

void APickupItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsCollected);
}

void APickupItemBase::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor);
	if (IsValid(Player))
	{
		if (!Player->IsLocallyControlled()) return;

		InteractableWidget->SetVisibility(true);
	}
}

void APickupItemBase::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor);
	if (IsValid(Player))
	{
		if (!Player->IsLocallyControlled()) return;

		InteractableWidget->SetVisibility(false);
	}
}

void APickupItemBase::Interact(ACharacter* User, const FHitResult& HitResult)
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
	if (!IsValid(Player)) return;

	Player->ServerRPCInteract(this, User, HitResult);
}

void APickupItemBase::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	if (bIsCollected) return;

	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
	if (!IsValid(Player) || !IsValid(Player->InvenComp)) return;

	bIsCollected = true;

	TArray<FInventoryItem>& Inventory = Player->InvenComp->Inventory;
	int32 ValidItem = 0;
	for (int32 i = 0; i < Inventory.Num(); ++i)
	{
		if (Inventory[i].ItemID != NAME_None)
		{
			ValidItem++;
		}
	}
	if (ValidItem < Inventory.Num())
	{
		Player->InvenComp->AddItem(GetItemID());
		Destroy();
	}
	else
	{
		bIsCollected = false;
		UE_LOG(LogTemp, Error, TEXT("Full Inventory"));
	}
}

FName APickupItemBase::GetItemID() const
{
	return ItemID;
}
