// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 몬스터 AI Blackboard 키 상수
 * 문자열 키 대신 상수를 사용하여 타입 안정성 확보
 */
namespace FCMonsterBBKeys
{
	// 공통 키 (모든 몬스터)
	inline const FName TargetPlayer = FName("TargetPlayer");
	inline const FName SeenPlayer = FName("SeenPlayer");
	inline const FName LastStimulusLocation = FName("LastStimulusLocation");

	// SoundHunter 전용 키
	inline const FName bHasLureTarget = FName("bHasLureTarget");
	inline const FName LureLocation = FName("LureLocation");
	inline const FName bHasHeardSound = FName("bHasHeardSound");
	inline const FName LastHeardLocation = FName("LastHeardLocation");

	// BlinkHunter 전용 키
	inline const FName IsStunned = FName("IsStunned");
	inline const FName FlashStunReady = FName("FlashStunReady");
	inline const FName IsBeingWatched = FName("IsBeingWatched");
}
