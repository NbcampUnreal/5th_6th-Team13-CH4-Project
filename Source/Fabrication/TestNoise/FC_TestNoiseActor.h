// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FC_TestNoiseActor.generated.h"

/**
 * 테스트용 Noise Actor
 * - 지정된 시간 후 MakeNoise 발생
 * - 블루프린트에서 Delay, Loudness 설정 가능
 */
UCLASS()
class FABRICATION_API AFC_TestNoiseActor : public AActor
{
	GENERATED_BODY()

public:
	AFC_TestNoiseActor();

protected:
	virtual void BeginPlay() override;

	/** MakeNoise 실행 */
	UFUNCTION()
	void TriggerNoise();

public:
	/** 시작 후 Noise 발생까지 대기 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Noise")
	float DelayTime = 3.0f;

	/** Noise 크기 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Noise", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Loudness = 1.0f;

	/** 반복 발생 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Noise")
	bool bRepeat = false;

	/** 반복 간격 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Noise", meta = (EditCondition = "bRepeat"))
	float RepeatInterval = 2.0f;

	/** 디버그 시각화 활성화 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test|Noise")
	bool bShowDebug = true;

private:
	FTimerHandle NoiseTimerHandle;
};
