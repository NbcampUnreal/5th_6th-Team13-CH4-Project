#include "Event/HE_Bell.h"

#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Player/FCPlayerCharacter.h"

AHE_Bell::AHE_Bell()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    SetHazardType(EHazardType::Bell);

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    BellBoundary = CreateDefaultSubobject<UBoxComponent>(TEXT("BellBoundary"));
    BellBoundary->SetupAttachment(Scene);
    BellBoundary->SetCollisionProfileName(TEXT("Boundary"));

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComp->SetupAttachment(Scene);

    BellSound = nullptr;
}

void AHE_Bell::BeginPlay()
{
    Super::BeginPlay();
}

void AHE_Bell::TriggerBell()
{
    if (!HasAuthority())
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Bell RINGS"));

    bBellActive = true;

    Multicast_PlayBellSound();

    GetWorld()->GetTimerManager().SetTimer(
        MovementCheckTimer,
        this,
        &AHE_Bell::CheckPlayersInArea,
        5.0f,
        true
    );

    GetWorld()->GetTimerManager().SetTimer(
        BellDurationTimer,
        this,
        &AHE_Bell::EndBellEvent,
        BellSound->GetDuration() - 2.0f,
        false
    );
}

void AHE_Bell::CheckPlayersInArea()
{
    if (!HasAuthority() || !bBellActive)
    {
        return;
    }

    TArray<AActor*> OverlappingActors;
    BellBoundary->GetOverlappingActors(
        OverlappingActors,
        AFCPlayerCharacter::StaticClass()
    );

    for (AActor* Actor : OverlappingActors)
    {
        AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
        if (!Player)
        {
            continue;
        }

        const float Speed = Player->GetVelocity().Size();

        if (Speed > 1.0f)
        {
            ApplyHazardDamageWithCooldown(Player);
        }
    }
}

void AHE_Bell::EndBellEvent()
{
    if (!HasAuthority())
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Bell Ended"));

    bBellActive = false;

    GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimer);

    const float RandomDelay = FMath::FRandRange(40.f, 90.f);
    GetWorld()->GetTimerManager().SetTimer(
        RandomBellTimer,
        this,
        &AHE_Bell::TriggerBell,
        RandomDelay,
        false
    );
}

void AHE_Bell::CheckPlayerVelocityAfterDelay()
{
    if (!HasAuthority())
    {
        return;
    }

    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors, AFCPlayerCharacter::StaticClass());

    for (AActor* Actor : OverlappingActors)
    {
        AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
        if (!Player)
        {
            continue;
        }

        const float Speed = Player->GetVelocity().Size();
        UE_LOG(LogTemp, Warning, TEXT("Player: %s | Speed: %.2f"), *Player->GetName(), Speed);

        if (Speed > 1.0f)
        {
            ApplyHazardDamageWithCooldown(Player);
            UE_LOG(LogTemp, Error, TEXT("💥 Bell Damage Applied: %s"), *Player->GetName());
        }
    }
}

void AHE_Bell::OnHazardRowReady()
{
    check(Row);

    GetWorld()->GetTimerManager().SetTimer(
        RandomBellTimer,
        this,
        &AHE_Bell::TriggerBell,
        Row->LoopInterval,
        false
    );
}


void AHE_Bell::Multicast_PlayBellSound_Implementation()
{
    if (BellSound)
    {
        UGameplayStatics::PlaySound2D(this, BellSound);
    }
}