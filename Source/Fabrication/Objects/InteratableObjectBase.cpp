#include "Objects/InteratableObjectBase.h"
#include "Fabrication.h"

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
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);

}

void AInteratableObjectBase::Interact(ACharacter* User, const FHitResult& HitResult)
{
	// 클라이언트 로직, User->ServerRPC로 서버 로직 호출
}

void AInteratableObjectBase::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	// 서버 로직
}

