// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/HE_Bell.h"

#include "Sound/SoundBase.h"
#include <Kismet/GameplayStatics.h>
#include "Components/BoxComponent.h"
#include <Player/FCPlayerCharacter.h>

AHE_Bell::AHE_Bell()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetHazardType(EHazardType::Bell);

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    BellBoundary = CreateDefaultSubobject<UBoxComponent>(TEXT("BellBoundary"));
    BellBoundary->SetupAttachment(Scene);
    BellBoundary->SetCollisionProfileName(TEXT("Boundary"));

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComp->SetupAttachment(Scene);

}

void AHE_Bell::BeginPlay()
{
    BellBoundary->OnComponentBeginOverlap.AddDynamic(this, &AHE_Bell::OnOverlapBeginBell);
}

void AHE_Bell::StartEvent()
{
    if (BellSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, BellSound, GetActorLocation());
    }

    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimer(
            BellPlayerTimer,
            this,
            &AHE_Bell::CheckPlayerVelocityAfterDelay,
            3.0f,
            false 
        );
    }
}

void AHE_Bell::CheckPlayerVelocityAfterDelay()
{
    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors, AFCPlayerCharacter::StaticClass());

    for (AActor* Actor : OverlappingActors)
    {
        if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor))
        {
            UE_LOG(LogTemp, Warning, TEXT("감지된 플레이어: %s, 속도: %f"), *Player->GetName(), Player->GetVelocity().Size());
            if (Player->GetVelocity().Size() > 1.0f)
            {
                ApplyHazardDamageWithCooldown(Player);
                UE_LOG(LogTemp, Warning, TEXT("벨에 걸림: %s 데미지!"), *Player->GetName());
            }
        }
    }
}

void AHE_Bell::OnOverlapBeginBell(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (HasAuthority() && bIsReady && OtherActor && OtherActor->IsA(AFCPlayerCharacter::StaticClass()))
    {
        bIsReady = false;

        UE_LOG(LogTemp, Warning, TEXT("벨 작동! 3초 뒤 속도 체크, 60초 뒤 재활성화됩니다."));

        if (BellSound)
        {
            Multicast_PlayBellSound();


            GetWorld()->GetTimerManager().SetTimer(
                BellPlayerTimer,
                this,
                &AHE_Bell::CheckPlayerVelocityAfterDelay,
                3.0f,
                false
            );
        }

        GetWorld()->GetTimerManager().SetTimer(
            BellTimer,
            this,
            &AHE_Bell::ResetBellReady,
            10.0f,
            false
        );
    }
}

void AHE_Bell::ResetBellReady()
{
    bIsReady = true;

    UE_LOG(LogTemp, Log, TEXT("벨이 다시 준비되었습니다."));
}

void AHE_Bell::Multicast_PlayBellSound_Implementation()
{
    if (BellSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, BellSound, GetActorLocation());
    }
}