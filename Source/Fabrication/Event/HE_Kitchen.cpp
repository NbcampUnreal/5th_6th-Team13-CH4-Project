#include "Event/HE_Kitchen.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FCPlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include <Fabrication.h>

AHE_Kitchen::AHE_Kitchen()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetHazardType(EHazardType::Kitchen);

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComp->SetupAttachment(RootScene);
    MeshComp->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);

    InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionTrigger"));
    InteractionTrigger->SetupAttachment(RootScene);
    InteractionTrigger->SetCollisionProfileName(TEXT("Trigger"));

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(RootScene);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractionWidget->SetDrawAtDesiredSize(true);

    EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
    EffectComp->SetupAttachment(RootScene);
    EffectComp->bAutoActivate = false;
}

void AHE_Kitchen::BeginPlay()
{
    Super::BeginPlay();

    InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AHE_Kitchen::OnOverlapBegin);
    InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &AHE_Kitchen::OnOverlapEnd);

    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(false);
    }
}

void AHE_Kitchen::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bIsDone) return;

    if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor))
    {
        if (Player->IsLocallyControlled())
        {
            InteractionWidget->SetVisibility(true);
        }
    }
}

void AHE_Kitchen::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor))
    {
        if (Player->IsLocallyControlled())
        {
            InteractionWidget->SetVisibility(false);
        }
    }
}

void AHE_Kitchen::Interact(ACharacter* User, const FHitResult& HitResult)
{
    if (bIsDone) return;

    if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User))
    {
        if (Player->IsLocallyControlled())
        {
            UE_LOG(LogTemp, Log, TEXT("Client: Attempting to Interact with Kitchen"));
        }

        Player->ServerRPCInteract(this, User, HitResult);
    }
}

void AHE_Kitchen::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
    if (!HasAuthority() || bIsDone) return;
    bIsDone = true;
    OnRep_IsDone();
    FVector SpawnPos = EffectComp->GetComponentLocation();

    ApplyEffect(EffectComp->Template, SpawnPos);

    float DamageAmount = 1.0f;
    UGameplayStatics::ApplyDamage(User, DamageAmount, User->GetController(), this, UDamageType::StaticClass());

    UE_LOG(LogTemp, Log, TEXT("Kitchen Event Executed: Damage Applied to %s"), *User->GetName());
}

void AHE_Kitchen::OnRep_IsDone()
{
    if (bIsDone && InteractionWidget)
    {
        InteractionWidget->SetVisibility(false);
    }
}

void AHE_Kitchen::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, bIsDone);
}