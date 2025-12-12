// Fill out your copyright notice in the Description page of Project Settings.

#include "Event/LevelEventManager.h"
#include "Event/BaseHazardEvent.h"
#include "GameMode/FCGameMode.h"

ULevelEventManager::ULevelEventManager()
{}

//void UEventManager::Initialize(FSubsystemCollectionBase& Collection)
//{
//    Super::Initialize(Collection);
//
//    // 맵에 있는 모든 이벤트 액터 찾기
//    for (TActorIterator<AEventActor> It(GetWorld()); It; ++It)
//    {
//        AEventActor* EventActor = *It;
//        if (EventActor)
//        {
//            EventActors.Add(EventActor);
//            EventActor->RegisterToManager(this);
//        }
//    }
//}

void ULevelEventManager::StartEventLoop(EHazardType Type)
{
    if (!HazardDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("No HazardDataTable assigned"));
        return;
    }

    FString ContextString;
    TArray<FC_HazardDataRow*> AllRows;
    HazardDataTable->GetAllRows(ContextString, AllRows);
    bIsSafe = true;
    for (auto* Row : AllRows)
    {
        if (Row->HazardType == Type)
        {
            // 바로 여기서 스폰
            SpawnHazardActor(Row);

            float Interval = Row->LoopInterval;

            GetWorld()->GetTimerManager().SetTimer(
                LoopHandle,
                this,
                &ULevelEventManager::StopEventLoop,
                Interval,
                false
            );

            CurrentEventType = Type;
            return;
        }
    }
}


void ULevelEventManager::StopEventLoop()
{
    UE_LOG(LogTemp, Warning, TEXT("[EventManager] StopEventLoop"));

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(LoopHandle);
    }
    bIsSafe = !bIsSafe;
}

void ULevelEventManager::TriggerRandomEvent(EHazardType Type)
{
    UE_LOG(LogTemp, Warning, TEXT("[EventManager] TriggerRandomEvent fired"));

    HazardType = Type;
    StartEventLoop(Type);
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
        HazardEvent->StartEvent();
        UE_LOG(LogTemp, Warning, TEXT("[EventManager] StartEvent() executed for %s"),
            *SpawnedActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Spawned Actor is not ABaseHazardEvent"));
    }

    
}
