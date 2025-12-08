#include "Objects/InteratableObjectBase.h"

AInteratableObjectBase::AInteratableObjectBase()
	: SceneComp(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComp);
	
}

void AInteratableObjectBase::Interact(ACharacter* User)
{
}

void AInteratableObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
}
