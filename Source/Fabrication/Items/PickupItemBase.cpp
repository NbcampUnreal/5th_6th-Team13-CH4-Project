#include "Items/PickupItemBase.h"

APickupItemBase::APickupItemBase()
	: SceneComp(nullptr)
	, StaticMeshComp(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComp);
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneComp);

}

void APickupItemBase::Interact(ACharacter* User, const FHitResult& HitResult)
{
}

void APickupItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}
