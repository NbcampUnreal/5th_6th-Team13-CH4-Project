// Fill out your copyright notice in the Description page of Project Settings.

#include "TestNoise/FC_TestNoiseActor.h"
#include "Fabrication.h"
#include "DrawDebugHelpers.h"
#include "Perception/AISense_Hearing.h"

AFC_TestNoiseActor::AFC_TestNoiseActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFC_TestNoiseActor::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 실행
	if (!HasAuthority()) return;

	if (bRepeat)
	{
		// 반복 타이머
		GetWorldTimerManager().SetTimer(
			NoiseTimerHandle,
			this,
			&AFC_TestNoiseActor::TriggerNoise,
			RepeatInterval,
			true,  // bLoop
			DelayTime  // FirstDelay
		);
	}
	else
	{
		// 1회 타이머
		GetWorldTimerManager().SetTimer(
			NoiseTimerHandle,
			this,
			&AFC_TestNoiseActor::TriggerNoise,
			DelayTime,
			false
		);
	}

	FC_LOG_NET(LogFCNet, Log, TEXT("TestNoiseActor Started - Delay: %.1fs, Loudness: %.2f, Repeat: %s"),
		DelayTime, Loudness, bRepeat ? TEXT("true") : TEXT("false"));
}

void AFC_TestNoiseActor::TriggerNoise()
{
	const FVector NoiseLocation = GetActorLocation();

	// AI Perception Hearing으로 직접 소리 발생 (일반 소리, 태그 없음)
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		NoiseLocation,
		Loudness,
		this,
		0.0f,      // MaxRange (0 = 무제한, Perception에서 처리)
		NAME_None  // 태그 없음 (일반 소리)
	);

	FC_LOG_NET(LogFCNet, Warning, TEXT("=== NOISE TRIGGERED === Location: %s, Loudness: %.2f"),
		*NoiseLocation.ToString(), Loudness);

#if WITH_EDITOR
	if (bShowDebug)
	{
		// 파란색 구체: 테스트 일반 소리
		DrawDebugSphere(GetWorld(), NoiseLocation, 100.0f, 16, FColor::Blue, false, 3.0f, 0, 3.0f);
		DrawDebugString(GetWorld(), NoiseLocation + FVector(0, 0, 120),
			FString::Printf(TEXT("TEST NOISE (%.1f)"), Loudness), nullptr, FColor::Blue, 3.0f);
	}
#endif
}
