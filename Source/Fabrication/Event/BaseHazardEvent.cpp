// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/BaseHazardEvent.h"
#include "Event/LevelEventManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void ABaseHazardEvent::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority()) return;

    // BeginPlay에서는 바로 쓰지 말고 Init으로 위임
    GetWorld()->GetTimerManager().SetTimerForNextTick(
        this,
        &ABaseHazardEvent::InitFromHazardRow
    );
}

void ABaseHazardEvent::ApplyEffect(UParticleSystem* Effect, FVector EffectLocation)
{
	// 서버 권한이 있을 때만 멀티캐스트 호출
	if (HasAuthority() && Effect)
	{
		Multicast_PlayEffect(Effect, EffectLocation);
	}
}

void ABaseHazardEvent::Multicast_PlayEffect_Implementation(UParticleSystem* Effect, FVector EffectLocation)
{
	if (Effect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Effect, EffectLocation);
	}
}

void ABaseHazardEvent::StopEffect()
{

}

void ABaseHazardEvent::SetHazardType(EHazardType Type)
{
	HazardType = Type;
}

EHazardType ABaseHazardEvent::GetHazardType()
{
	return HazardType;
}

void ABaseHazardEvent::InitFromHazardRow()
{
    if (Row) return;

    if (ULevelEventManager* Manager = GetWorld()->GetSubsystem<ULevelEventManager>())
    {
        if (!Manager->bInitialized)
        {
            GetWorld()->GetTimerManager().SetTimerForNextTick(
                this,
                &ABaseHazardEvent::InitFromHazardRow
            );
            return;
        }

        Row = Manager->GetHazardRow(HazardType);
    }

    if (!Row)
    {
        UE_LOG(LogTemp, Error,
            TEXT("[%s] HazardRow nullptr (Type=%d)"),
            *GetName(),
            (int32)HazardType
        );
        return;
    }

    OnHazardRowReady();
}

const FC_HazardDataRow* ABaseHazardEvent::GetMyHazardRow() const
{
	if (UWorld* World = GetWorld())
	{
		if (ULevelEventManager* Manager =
			World->GetSubsystem<ULevelEventManager>())
		{
			return Manager->GetHazardRow(HazardType);
		}
	}
	return nullptr;
}

bool ABaseHazardEvent::CanApplyDamage()
{
    if (!Row) return false;

    float CurrentTime = GetWorld()->GetTimeSeconds();

    return (CurrentTime - LastDamageTime) >= Row->DamageDelay;
}

void ABaseHazardEvent::ApplyHazardDamageWithCooldown(AActor* TargetActor)
{
    if (!TargetActor || !HasAuthority()) return;

    if (!Row)
    {
        UE_LOG(LogTemp, Error, TEXT("Painting: Cannot find Hazard Data Row!"));
        return;
    }
    float CurrentTime = GetWorld()->GetTimeSeconds();

    if (LastDamageTimeMap.Contains(TargetActor))
    {
        float LastTime = LastDamageTimeMap[TargetActor];

        if (CurrentTime - LastTime < Row->DamageDelay)
        {
            return;
        }
    }

    UGameplayStatics::ApplyDamage(
        TargetActor,
        Row->DamageAmount,
		nullptr,
        this,
        UDamageType::StaticClass()
    );

    LastDamageTimeMap.Add(TargetActor, CurrentTime);
}

