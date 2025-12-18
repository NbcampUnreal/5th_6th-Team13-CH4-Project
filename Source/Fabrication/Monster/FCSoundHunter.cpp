// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/FCSoundHunter.h"
#include "Net/UnrealNetwork.h"

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

	bHasLureTarget = true;
	LureLocation = Location;

	UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Lure target set at: %s"), *Location.ToString());
}

void AFCSoundHunter::ClearLureTarget()
{
	// 서버에서만 호출되어야 함
	if (!HasAuthority()) return;

	bHasLureTarget = false;
	LureLocation = FVector::ZeroVector;

	UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Lure target cleared"));
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

	UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Heard sound at: %s"), *Location.ToString());
}

void AFCSoundHunter::ClearHeardSound()
{
	// 서버에서만 호출되어야 함
	if (!HasAuthority()) return;

	bHasHeardSound = false;
	LastHeardLocation = FVector::ZeroVector;

	UE_LOG(LogTemp, Log, TEXT("[SoundHunter] Heard sound cleared"));
}

bool AFCSoundHunter::HasArrivedAtHeardLocation() const
{
	if (!bHasHeardSound) return false;

	const float Distance = FVector::Dist(GetActorLocation(), LastHeardLocation);
	return Distance <= LureArrivalDistance;  // Lure와 같은 도착 거리 사용
}