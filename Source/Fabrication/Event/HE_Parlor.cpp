// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/HE_Parlor.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

AHE_Parlor::AHE_Parlor()
{
    PrimaryActorTick.bCanEverTick = true;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AHE_HazardArea::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AHE_HazardArea::OnOverlapEnd);

    bPlayerInside = false;
    TimeInside = 0.f;
}

void AHE_Parlor::BeginPlay()
{
    Super::BeginPlay();
}

void AHE_Parlor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bPlayerInside)
    {
        TimeInside += DeltaTime;
        if (TimeInside >= 3.f && SequenceToPlay && !LevelSequencePlayer)
        {
            // Level Sequence 생성 및 재생
            FMovieSceneSequencePlaybackSettings Settings;
            LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
                GetWorld(),
                SequenceToPlay,
                Settings,
                SequenceActor
            );

            if (LevelSequencePlayer)
            {
                LevelSequencePlayer->Play();
            }

            // 한 번만 실행
            bPlayerInside = false;
        }
    }
}

void AHE_Parlor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        bPlayerInside = true;
        TimeInside = 0.f;
    }
}

void AHE_Parlor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        bPlayerInside = false;
        TimeInside = 0.f;
    }
}