// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/FCSoundHunter.h"
#include "Net/UnrealNetwork.h"
#include "Fabrication.h"

AFCSoundHunter::AFCSoundHunter()
{
	// 기본값 초기화
	bHasLureTarget = false;
	LureLocation = FVector::ZeroVector;
	LastHeardLocation = FVector::ZeroVector;
	bHasHeardSound = false;
}

void AFCSoundHunter::BeginPlay()
{
	Super::BeginPlay();
}

void AFCSoundHunter::ApplyMonsterData()
{
	// 부모 클래스의 공통 스탯 적용 먼저 호출
	Super::ApplyMonsterData();

	if (!bDataTableLoaded)
	{
		return;
	}

	// SoundHunter 전용 스탯 적용
	HearingRange = CachedMonsterData.HearingRadius;
	LureDuration = CachedMonsterData.LureDuration;
	LureArrivalDistance = CachedMonsterData.LureArrivalDistance;

	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] SoundHunter 스탯 적용 - HearingRange: %.0f, LureDuration: %.1f, ArrivalDist: %.0f"),
		*GetName(), HearingRange, LureDuration, LureArrivalDistance);
}

void AFCSoundHunter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// SoundHunter 전용 Replicated 변수
	DOREPLIFETIME(AFCSoundHunter, bHasLureTarget);
	DOREPLIFETIME(AFCSoundHunter, LureLocation);
	DOREPLIFETIME(AFCSoundHunter, LastHeardLocation);
	DOREPLIFETIME(AFCSoundHunter, bHasHeardSound);
}

void AFCSoundHunter::SetLureTarget(const FVector& Location)
{
	// 서버에서만 호출되어야 함 (AIController가 호출)
	if (!HasAuthority()) return;

	// [버그 수정] Lure 감지 시 현재 재생 중인 Investigate 몽타주 즉시 정지
	Multicast_StopInvestigateAnim();

	// [버그 수정] Lure 감지 시 Investigate(HeardSound) 상태도 초기화
	bHasHeardSound = false;
	LastHeardLocation = FVector::ZeroVector;

	bHasLureTarget = true;
	LureLocation = Location;

	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Lure target set at: %s (HeardSound cleared)"), *GetName(), *Location.ToString());
}

void AFCSoundHunter::ClearLureTarget()
{
	// 서버에서만 호출되어야 함
	if (!HasAuthority()) return;

	bHasLureTarget = false;
	LureLocation = FVector::ZeroVector;

	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Lure target cleared"), *GetName());
}

bool AFCSoundHunter::HasArrivedAtLure() const
{
	if (!bHasLureTarget) return false;

	const float Distance = FVector::Dist(GetActorLocation(), LureLocation);
	return Distance <= LureArrivalDistance;
}

void AFCSoundHunter::SetHeardSound(const FVector& Location)
{
	// 서버에서만 호출되어야 함 (AIController가 호출)
	if (!HasAuthority()) return;

	bHasHeardSound = true;
	LastHeardLocation = Location;

	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Heard sound at: %s"), *GetName(), *Location.ToString());
}

void AFCSoundHunter::ClearHeardSound()
{
	// 서버에서만 호출되어야 함
	if (!HasAuthority()) return;

	bHasHeardSound = false;
	LastHeardLocation = FVector::ZeroVector;

	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Heard sound cleared"), *GetName());
}

bool AFCSoundHunter::HasArrivedAtHeardLocation() const
{
	if (!bHasHeardSound) return false;

	const float Distance = FVector::Dist(GetActorLocation(), LastHeardLocation);
	return Distance <= LureArrivalDistance;  // Lure와 같은 도착 거리 사용
}