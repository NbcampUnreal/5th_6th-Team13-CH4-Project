// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/HE_Kitchen.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Event/Widget/HE_KitchenWidget.h" 

AHE_Kitchen::AHE_Kitchen()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	KitchenBox = CreateDefaultSubobject<UBoxComponent>(TEXT("KitchenBox"));
	KitchenBox->SetupAttachment(Scene);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(Scene);

	// 기본 설정만 C++에서
	MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
	MeshComp->SetGenerateOverlapEvents(true);

	InteractionWidgetComp =
		CreateDefaultSubobject<UWidgetComponent>(TEXT("HE_KitchenWidget"));
	InteractionWidgetComp->SetupAttachment(Scene);

	InteractionWidgetComp->SetWidgetSpace(EWidgetSpace::World);
	InteractionWidgetComp->SetDrawSize(FVector2D(200.f, 50.f));
	InteractionWidgetComp->SetVisibility(false);

	InteractionWidgetComp->SetWidgetClass(InteractionWidgetClass);

	UE_LOG(LogTemp, Warning, TEXT("WidgetClass = %s"),
		*GetNameSafe(InteractionWidgetComp->GetWidgetClass()));

	bCanInteract = false;

	bReplicates = true;
	SetReplicateMovement(true);
}

void AHE_Kitchen::StartEvent()
{
}

void AHE_Kitchen::EndEvent()
{
}

void AHE_Kitchen::BeginPlay()
{
	Super::BeginPlay();

	KitchenBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&AHE_Kitchen::OnBeginOverlap_Kitchen
	);

	KitchenBox->OnComponentEndOverlap.AddDynamic(
		this,
		&AHE_Kitchen::OnEndOverlap_Kitchen
	);
}

void AHE_Kitchen::OnBeginOverlap_Kitchen(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bCanInteract = true;

	InteractionWidgetComp->SetVisibility(true);

	UE_LOG(LogTemp, Warning, TEXT("Overlap!!"));
	if (UHE_KitchenWidget* Widget =
		Cast<UHE_KitchenWidget>(InteractionWidgetComp->GetUserWidgetObject()))
	{
		Widget->ShowInteract();
		Widget->UpdateInteractText(FText::FromString("Press E"));
	}
}

void AHE_Kitchen::OnEndOverlap_Kitchen(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCanInteract = false;
	UE_LOG(LogTemp, Warning, TEXT("No Overlap!!"));
	if (UHE_KitchenWidget* Widget =
		Cast<UHE_KitchenWidget>(InteractionWidgetComp->GetUserWidgetObject()))
	{
		Widget->HideInteract();
	}
}
