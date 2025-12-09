#include "Objects/WardrobeHideSpot.h"
#include "Components/BoxComponent.h"

AWardrobeHideSpot::AWardrobeHideSpot()
{
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(SceneComp);
	HideSpot = CreateDefaultSubobject<UBoxComponent>(TEXT("HideSpot"));
	HideSpot->SetupAttachment(SceneComp);
}

void AWardrobeHideSpot::Interact(ACharacter* User, const FHitResult& HitResult)
{
}
