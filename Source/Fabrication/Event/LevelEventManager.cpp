// Fill out your copyright notice in the Description page of Project Settings.

#include "Event/LevelEventManager.h"

ULevelEventManager::ULevelEventManager()
{
}

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
            // 타이머 시작
            float Interval = Row->LoopInterval;

            GetWorld()->GetTimerManager().SetTimer(
                LoopHandle,
                this,
                &ULevelEventManager::StopEventLoop,
                Interval,
                true
            );
            CurrentEventType = Type;
            return;
        }
    }
}

void ULevelEventManager::StopEventLoop()
{
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