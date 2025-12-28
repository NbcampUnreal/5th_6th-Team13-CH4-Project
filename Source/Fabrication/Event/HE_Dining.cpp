#include "Event/HE_Dining.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/FCPlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AHE_Dining::AHE_Dining()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    SetHazardType(EHazardType::Dining);

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionTrigger"));
    InteractionTrigger->SetupAttachment(Root);
    InteractionTrigger->SetCollisionProfileName(TEXT("Trigger"));
    InteractionTrigger->SetHiddenInGame(false);
}

void AHE_Dining::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
    {
        InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AHE_Dining::OnOverlapBegin);
    }
}

FVector AHE_Dining::GetRandomPointInVolume_Sphere() const
{
    return InteractionTrigger->GetComponentLocation();
}

void AHE_Dining::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (bHasHappened || !HasAuthority() || !ItemClassToSpawn) return;
    UE_LOG(LogTemp, Warning, TEXT("Dining Overlap"));
    if (Cast<AFCPlayerCharacter>(OtherActor))
    {
        bHasHappened = true;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = nullptr;

        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        FVector Loc = GetRandomPointInVolume_Sphere();
        FRotator Rot = InteractionTrigger->GetComponentRotation();

        AActor* NewItem = GetWorld()->SpawnActor<AActor>(ItemClassToSpawn, Loc, Rot, SpawnParams);

        if (NewItem)
        {
            Multicast_DropItem(NewItem);
        }
    }
}

void AHE_Dining::Multicast_DropItem_Implementation(AActor* SpawnedItem)
{
    if (!SpawnedItem) return;

    if (ItemSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ItemSound, SpawnedItem->GetActorLocation());
    }

    TArray<UStaticMeshComponent*> MeshComps;
    SpawnedItem->GetComponents<UStaticMeshComponent>(MeshComps);
    UE_LOG(LogTemp, Warning, TEXT("DropItem"));
    for (UStaticMeshComponent* Mesh : MeshComps)
    {
        if (Mesh)
        {
            Mesh->SetSimulatePhysics(true);
            Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Mesh->AddImpulse(FVector(0.f, 0.f, -30.0f));
        }
    }
}