// Fill out your copyright notice in the Description page of Project Settings.

#include "Event/LevelEventManager.h"
#include "Event/BaseHazardEvent.h"
#include "GameMode/FCGameMode.h"

ULevelEventManager::ULevelEventManager()
{}
void ULevelEventManager::StartEventLoop(EHazardType Type)
{
    const FC_HazardDataRow* Row = GetHazardRow(Type);
    if (!Row)
    {
        UE_LOG(LogTemp, Error, TEXT("[EventManager] No Hazard Row"));
        return;
    }

    SpawnHazardActor(Row);
}


void ULevelEventManager::StopEventLoop()
{
    UE_LOG(LogTemp, Warning, TEXT("[EventManager] StopEventLoop"));

    //if (GetWorld())
    //{
    //    GetWorld()->GetTimerManager().ClearTimer(LoopHandle);
    //}

    //nSafeStateChanged.Broadcast(bIsSafe);

    StartEventLoop(CurrentEventType);
}

void ULevelEventManager::TriggerRandomEvent(EHazardType Type)
{
    UE_LOG(LogTemp, Warning, TEXT("[EventManager] TriggerRandomEvent fired"));

    const FC_HazardDataRow* Row = GetHazardRow(Type);
    if (!Row)
    {
        UE_LOG(LogTemp, Error, TEXT("No Hazard Row Found"));
        return;
    }

    //SpawnHazardActor(Row);
}

void ULevelEventManager::Damage()
{

}

void ULevelEventManager::SpawnHazardActor(const FC_HazardDataRow* Row)
{
    if (!Row || !Row->HazardActorClass) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    SpawnedActor = World->SpawnActor<ABaseHazardEvent>(
        Row->HazardActorClass,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (!SpawnedActor)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn Hazard Actor"));
        return;
    }

    if (ABaseHazardEvent* HazardEvent = Cast<ABaseHazardEvent>(SpawnedActor))
    {
        //HazardEvent->StartEvent();
        UE_LOG(LogTemp, Warning, TEXT("[EventManager] StartEvent() executed for %s"),
            *SpawnedActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Spawned Actor is not ABaseHazardEvent"));
    }

    
}

const FC_HazardDataRow* ULevelEventManager::GetHazardRow(EHazardType Type) const
{
    if (!HazardDataTable) return nullptr;

    TArray<const FC_HazardDataRow*> Candidates;

    for (const FName& RowName : HazardDataTable->GetRowNames())
    {
        const FC_HazardDataRow* Row =
            HazardDataTable->FindRow<FC_HazardDataRow>(RowName, TEXT("GetHazardRow"));

        if (Row && Row->HazardType == Type)
        {
            Candidates.Add(Row);
        }
    }

    if (Candidates.Num() == 0) return nullptr;

    return Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
}