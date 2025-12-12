// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/DataTable.h"
#include "FC_HazardDataRow.h"
#include "GameMode/FCGameMode.h"
#include "LevelEventManager.generated.h"

/**
 * 
 */
UCLASS()
class FABRICATION_API ULevelEventManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	ULevelEventManager();

public:
    // 이벤트 시작
    void StartEventLoop(EHazardType Type);

    // 이벤트 정지
    void StopEventLoop();

    // 랜덤 이벤트 트리거
    void TriggerRandomEvent(EHazardType Type);

    //이벤트 내용
    //void HazardEvent();
    void SpawnHazardActor(const FC_HazardDataRow* Row);
    // 데미지 처리
    void Damage();

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
    UDataTable* HazardDataTable;

    EHazardType CurrentEventType = EHazardType::None;

    EHazardType HazardType = EHazardType::None;

    FTimerHandle LoopHandle;

    AActor* SpawnedActor;

    AFCGameMode* GM;

    bool bIsSafe;
	
};
