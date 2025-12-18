// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FC_TestLureNoiseActor.generated.h"

/**
 * 테스트용 Lure Noise Actor
 * - 지정된 시간 후 "Lure" 태그가 붙은 MakeNoise 발생
 * - SoundHunter가 최우선으로 반응해야 함
 */
UCLASS()
class FABRICATION_API AFC_TestLureNoiseActor : public AActor
{
	GENERATED_BODY()

public:
	AFC_TestLureNoiseActor();

protected:
	virtual void BeginPlay() override;

	/** MakeNoise 실행 (Lure 태그) */
	UFUNCTION()
	void TriggerLureNoise();

public:
	/** 시작 후 Noise 발생까지 대기 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Lure")
	float DelayTime = 5.0f;

	/** Noise 크기 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Lure", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Loudness = 1.0f;

	/** 반복 발생 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Lure")
	bool bRepeat = false;

	/** 반복 간격 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Lure", meta = (EditCondition = "bRepeat"))
	float RepeatInterval = 5.0f;

	/** 디버그 시각화 활성화 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Lure")
	bool bShowDebug = true;

private:
	FTimerHandle LureTimerHandle;
};
