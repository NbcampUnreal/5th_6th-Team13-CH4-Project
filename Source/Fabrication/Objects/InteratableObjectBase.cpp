#include "Objects/InteratableObjectBase.h"
#include "Player/FCPlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Fabrication.h"

AInteratableObjectBase::AInteratableObjectBase()
	: SceneComp(nullptr)
	, StaticMeshComp(nullptr)
	, InteractableWidget(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneComp);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_PickUp, ECR_Ignore);

	/*
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);
	*/

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	BoxComp->SetupAttachment(SceneComp);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	//BoxComp->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);

	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableUI"));
	InteractableWidget->SetupAttachment(SceneComp);
	InteractableWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractableWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractableWidget->SetCollisionResponseToAllChannels(ECR_Ignore);

	TestIneractableWidget = CreateDefaultSubobject<UBillboardComponent>(TEXT("TestUI")); //
	TestIneractableWidget->SetupAttachment(SceneComp);
	TestIneractableWidget->SetHiddenInGame(true);
	TestIneractableWidget->bIsScreenSizeScaled = true;
	TestIneractableWidget->ScreenSize = 0.02f;
}

void AInteratableObjectBase::BeginPlay()
{
	Super::BeginPlay();

	InteractableWidget->SetVisibility(false);

	if (!BoxComp->OnComponentBeginOverlap.IsAlreadyBound(this, &AInteratableObjectBase::OnItemOverlap))
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AInteratableObjectBase::OnItemOverlap);
	}
	if (!BoxComp->OnComponentEndOverlap.IsAlreadyBound(this, &AInteratableObjectBase::OnItemEndOverlap))
	{
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &AInteratableObjectBase::OnItemEndOverlap);
	}

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

void AInteratableObjectBase::OnItemOverlap(
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
		TestIneractableWidget->SetHiddenInGame(false);
	}
}

void AInteratableObjectBase::OnItemEndOverlap(
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
		TestIneractableWidget->SetHiddenInGame(true);
	}
}