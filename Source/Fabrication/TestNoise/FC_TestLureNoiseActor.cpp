// Fill out your copyright notice in the Description page of Project Settings.

#include "TestNoise/FC_TestLureNoiseActor.h"
#include "Fabrication.h"
#include "DrawDebugHelpers.h"
#include "Perception/AISense_Hearing.h"

AFC_TestLureNoiseActor::AFC_TestLureNoiseActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFC_TestLureNoiseActor::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 실행
	if (!HasAuthority()) return;

	if (bRepeat)
	{
		// 반복 타이머
		GetWorldTimerManager().SetTimer(
			LureTimerHandle,
			this,
			&AFC_TestLureNoiseActor::TriggerLureNoise,
			RepeatInterval,
			true,  // bLoop
			DelayTime  // FirstDelay
		);
	}
	else
	{
		// 1회 타이머
		GetWorldTimerManager().SetTimer(
			LureTimerHandle,
			this,
			&AFC_TestLureNoiseActor::TriggerLureNoise,
			DelayTime,
			false
		);
	}

	FC_LOG_NET(LogFCNet, Log, TEXT("TestLureNoiseActor Started - Delay: %.1fs, Loudness: %.2f, Repeat: %s"),
		DelayTime, Loudness, bRepeat ? TEXT("true") : TEXT("false"));
}

void AFC_TestLureNoiseActor::TriggerLureNoise()
{
	const FVector NoiseLocation = GetActorLocation();

	// Lure 태그가 붙은 소리 발생
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		NoiseLocation,
		Loudness,
		this,
		0.0f,  // MaxRange (0 = 무제한, Perception에서 처리)
		FName("Lure")  // 태그: Lure
	);

	FC_LOG_NET(LogFCNet, Warning, TEXT("=== LURE NOISE TRIGGERED === Location: %s, Loudness: %.2f"),
		*NoiseLocation.ToString(), Loudness);

#if WITH_EDITOR
	if (bShowDebug)
	{
		// 초록색 구체: 테스트 Lure 소리
		DrawDebugSphere(GetWorld(), NoiseLocation, 120.0f, 16, FColor::Green, false, 4.0f, 0, 4.0f);
		DrawDebugString(GetWorld(), NoiseLocation + FVector(0, 0, 150),
			FString::Printf(TEXT("TEST LURE! (%.1f)"), Loudness), nullptr, FColor::Green, 4.0f);
	}
#endif
}
