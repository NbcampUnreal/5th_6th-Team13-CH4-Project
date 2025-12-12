#include "Objects/InteratableObjectBase.h"

AInteratableObjectBase::AInteratableObjectBase()
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

void AInteratableObjectBase::Interact(ACharacter* User, const FHitResult& HitResult)
{
}

FName AInteratableObjectBase::GetItemID() const
{
	return TEXT("Object");
}
