// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/HE_Parlor.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"

AHE_Parlor::AHE_Parlor()
{
    PrimaryActorTick.bCanEverTick = true;

    SetHazardType(EHazardType::Parior);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AHE_Parlor::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AHE_Parlor::OnOverlapEnd);

    PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
    PostProcessComponent->SetupAttachment(RootComponent);
    PostProcessComponent->bUnbound = true;
    PostProcessComponent->bEnabled = false; // 처음엔 비활성 -> 나중에 활성화 할 예정

    bPlayerInside = false;
    bPostProcessActivated = false;

    TimeInside = 0.f;

    Elapsed = 0.f;
    TotalDuration = 10.f;
    RequiredOverlapTime = 3.0f;
    Alpha = 0.f;
    bFadingIn = false;
}

void AHE_Parlor::BeginPlay()
{
    Super::BeginPlay();

    if (PostProcessMaterial)
    {
        PostProcessMID = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);

        PostProcessComponent->Settings.WeightedBlendables.Array.Add(
            FWeightedBlendable(1.f, PostProcessMID)
        );

        PostProcessMID->SetScalarParameterValue(TEXT("Intensity"), 0.f);
    }

}

void AHE_Parlor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bPlayerInside && !bPostProcessActivated)
    {
        TimeInside += DeltaTime;

        if (TimeInside >= RequiredOverlapTime)
        {
            PostProcessComponent->bEnabled = true;

            bPostProcessActivated = true;
            bFadingIn = true;

            Elapsed = 0.f;
            Alpha = 0.f;
        }
    }

    if (bFadingIn && PostProcessMID)
    {
        Elapsed += DeltaTime;

        float t = FMath::Clamp(Elapsed / TotalDuration, 0.f, 1.f);

        float easedT = t * t;

        Alpha = FMath::Lerp(0.f, 1.f, easedT);

        PostProcessMID->SetScalarParameterValue(TEXT("Intensity"), Alpha);

        if (t >= 1.f)
        {
            Alpha = 1.f;
            PostProcessMID->SetScalarParameterValue(TEXT("Intensity"), Alpha);
            bFadingIn = false;
        }
    }
}


void AHE_Parlor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
    {
        bPlayerInside = true;
        TimeInside = 0.f;
    }
}

void AHE_Parlor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
    {
        bPlayerInside = false;
        bPostProcessActivated = false;
        bFadingIn = false;

        TimeInside = 0.f;
        Elapsed = 0.f;
        Alpha = 0.f;

        if (PostProcessMID)
        {
            PostProcessMID->SetScalarParameterValue(TEXT("Intensity"), 0.f);
        }

        PostProcessComponent->bEnabled = false;
    }
}