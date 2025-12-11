// Fill out your copyright notice in the Description page of Project Settings.

#include "Event/LevelEventManager.h"

ULevelEventManager::ULevelEventManager()
{
}

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

    for (auto* Row : AllRows)
    {
        if (Row->HazardType == Type)
        {
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