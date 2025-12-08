#include "Items/PickupItemBase.h"

APickupItemBase::APickupItemBase()
	: SceneComp(nullptr)
	, StaticMeshComp(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComp);
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneComp);

}

void APickupItemBase::Interact(ACharacter* User)
{
}

void APickupItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}
