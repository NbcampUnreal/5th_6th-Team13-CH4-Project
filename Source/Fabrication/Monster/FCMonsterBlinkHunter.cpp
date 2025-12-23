// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/FCMonsterBlinkHunter.h"
#include "Player/FCPlayerCharacter.h"
#include "Flash/FlashLight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Fabrication.h"

AFCMonsterBlinkHunter::AFCMonsterBlinkHunter()
{
	// 기본 상태 초기화
	bIsBeingWatched = false;
	FlashExposureTime = 0.0f;
	CachedMoveSpeed = 0.0f;
	bIsFrozen = false;

	// 기본 이동 속도를 Unseen 속도로 설정
	MoveSpeed_Normal = MoveSpeed_Unseen;
}

void AFCMonsterBlinkHunter::BeginPlay()
{
	Super::BeginPlay();

	// 초기 속도 캐싱 (BT에서 설정하기 전 기본값)
	CachedMoveSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// 초기 액터 캐시 갱신
	UpdateActorCaches();
}

bool AFCMonsterBlinkHunter::ShouldUpdateCache() const
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	return (CurrentTime - LastCacheUpdateTime) >= CacheUpdateInterval;
}

void AFCMonsterBlinkHunter::UpdateActorCaches()
{
	// 캐시 초기화
	CachedFlashLights.Empty();
	CachedPlayers.Empty();

	// FlashLight 캐싱
	TArray<AActor*> FlashLightActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlashLight::StaticClass(), FlashLightActors);
	for (AActor* Actor : FlashLightActors)
	{
		if (AFlashLight* FlashLight = Cast<AFlashLight>(Actor))
		{
			CachedFlashLights.Add(FlashLight);
		}
	}

	// PlayerCharacter 캐싱
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFCPlayerCharacter::StaticClass(), PlayerActors);
	for (AActor* Actor : PlayerActors)
	{
		if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor))
		{
			CachedPlayers.Add(Player);
		}
	}

	// 갱신 시간 업데이트
	LastCacheUpdateTime = GetWorld()->GetTimeSeconds();
}

void AFCMonsterBlinkHunter::SetMovementSpeed(float NewSpeed)
{
	if (!HasAuthority()) return;

	if (bIsFrozen)
	{
		// Frozen 상태에서는 캐시만 업데이트 (실제 속도는 0 유지)
		CachedMoveSpeed = NewSpeed;
	}
	else
	{
		// 정상 상태에서는 실제 속도 변경 + 캐시 업데이트
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
		CachedMoveSpeed = NewSpeed;
	}
}

void AFCMonsterBlinkHunter::SetFrozen(bool bFreeze)
{
	if (!HasAuthority()) return;

	if (bFreeze && !bIsFrozen)
	{
		// Freeze 시작: 현재 속도 캐싱 후 0으로 설정
		CachedMoveSpeed = GetCharacterMovement()->MaxWalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed_Frozen;
		bIsFrozen = true;
	}
	else if (!bFreeze && bIsFrozen)
	{
		// Freeze 해제: 캐싱된 속도로 복원
		GetCharacterMovement()->MaxWalkSpeed = CachedMoveSpeed;
		bIsFrozen = false;
	}
}

void AFCMonsterBlinkHunter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 관찰 상태 복제 (클라이언트 애니메이션용)
	DOREPLIFETIME(AFCMonsterBlinkHunter, bIsBeingWatched);
}

void AFCMonsterBlinkHunter::ApplyMonsterData()
{
	// 부모 클래스 공통 스탯 적용
	Super::ApplyMonsterData();

	if (!bDataTableLoaded)
	{
		return;
	}

	// BlinkHunter 전용 스탯 적용 - Flash
	FlashExposureThreshold = CachedMonsterData.FlashExposureThreshold;
	FlashStunDuration = CachedMonsterData.FlashStunDuration;

	// BlinkHunter 전용 스탯 적용 - Gaze (시선)
	PlayerViewAngle = CachedMonsterData.PlayerViewAngle;
	SightCheckDistance = CachedMonsterData.GazeCheckDistance;
	MoveSpeed_Frozen = CachedMonsterData.MoveSpeed_Frozen;
	MoveSpeed_Unseen = CachedMonsterData.MoveSpeed_Unseen;

	// MoveSpeed_Normal을 Unseen 속도로 동기화 (관찰받지 않을 때가 기본 상태)
	MoveSpeed_Normal = MoveSpeed_Unseen;

	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] BlinkHunter 스탯 적용 - Flash: %.1f/%.1f, Gaze: %.0f/%.0f, Speed: Frozen=%.0f Unseen=%.0f"),
		*GetName(), FlashExposureThreshold, FlashStunDuration, PlayerViewAngle, SightCheckDistance, MoveSpeed_Frozen, MoveSpeed_Unseen);
}

bool AFCMonsterBlinkHunter::IsBeingWatchedByPlayers()
{
	// 캐시 갱신이 필요하면 갱신
	if (ShouldUpdateCache())
	{
		UpdateActorCaches();
	}

	// 캐싱된 플레이어 목록에서 한 명이라도 바라보고 있으면 true
	for (const TWeakObjectPtr<AFCPlayerCharacter>& WeakPlayer : CachedPlayers)
	{
		if (AFCPlayerCharacter* Player = WeakPlayer.Get())
		{
			if (IsPlayerLookingAtMe(Player))
			{
				return true;
			}
		}
	}

	return false;
}

bool AFCMonsterBlinkHunter::CheckConeLineOfSight(
	const FVector& SourceLocation,
	const FVector& SourceForward,
	float MaxDistance,
	float ConeAngleDegrees,
	AActor* IgnoredActor
) const
{
	// 1. 거리 체크
	const float Distance = FVector::Dist(SourceLocation, GetActorLocation());
	if (Distance > MaxDistance)
	{
		return false;
	}

	// 2. DotProduct로 원뿔 각도 체크
	const FVector ToMonster = (GetActorLocation() - SourceLocation).GetSafeNormal();
	const float DotProduct = FVector::DotProduct(SourceForward, ToMonster);
	const float AngleCosine = FMath::Cos(FMath::DegreesToRadians(ConeAngleDegrees));

	if (DotProduct < AngleCosine)
	{
		return false;
	}

	// 3. LineTrace로 장애물 체크
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	if (IgnoredActor)
	{
		CollisionParams.AddIgnoredActor(IgnoredActor);
	}

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		SourceLocation,
		GetActorLocation(),
		ECC_Visibility,
		CollisionParams
	);

	// Hit가 없으면 (장애물 없음) true
	return !bHit;
}

bool AFCMonsterBlinkHunter::IsPlayerLookingAtMe(AFCPlayerCharacter* Player)
{
	if (!Player) return false;

	// 플레이어의 시선 방향 가져오기
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	Player->GetActorEyesViewPoint(PlayerViewLocation, PlayerViewRotation);
	const FVector PlayerForward = PlayerViewRotation.Vector();

	// 공통 함수로 체크
	return CheckConeLineOfSight(
		PlayerViewLocation,
		PlayerForward,
		SightCheckDistance,
		PlayerViewAngle,
		Player
	);
}

bool AFCMonsterBlinkHunter::IsExposedToFlash()
{
	// 캐시 갱신이 필요하면 갱신
	if (ShouldUpdateCache())
	{
		UpdateActorCaches();
	}

	// 캐싱된 FlashLight 목록에서 체크
	for (const TWeakObjectPtr<AFlashLight>& WeakFlashLight : CachedFlashLights)
	{
		AFlashLight* FlashLight = WeakFlashLight.Get();
		if (!FlashLight) continue;

		// [멀티플레이] Owner 플레이어의 bFlashLightOn 상태로 체크 (SpotLight Visibility는 복제 안 됨)
		AFCPlayerCharacter* OwnerPlayer = Cast<AFCPlayerCharacter>(FlashLight->GetOwner());
		bool bFlashOn = false;
		if (OwnerPlayer)
		{
			bFlashOn = OwnerPlayer->bFlashLightOn;
		}

		// FlashLight의 SpotLight 컴포넌트 찾기
		TArray<USpotLightComponent*> SpotLights;
		FlashLight->GetComponents<USpotLightComponent>(SpotLights);

		for (USpotLightComponent* SpotLight : SpotLights)
		{
			// [수정] SpotLight->IsVisible() 대신 bFlashLightOn 사용 (서버에서 복제된 값)
			if (SpotLight && bFlashOn && IsExposedToSpotLight(SpotLight))
			{
				FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Flash 빛에 노출됨! FlashLight: %s"), *GetName(), *FlashLight->GetName());
				return true;
			}
		}
	}

	return false;
}

void AFCMonsterBlinkHunter::UpdateFlashExposureTime(float DeltaTime, bool bExposed)
{
	float PrevTime = FlashExposureTime;

	if (bExposed)
	{
		// Flash 빛에 노출되면 시간 누적
		FlashExposureTime += DeltaTime;

		// [디버그] 노출 시간 증가 로그 (0.5초마다)
		if (FMath::FloorToInt(FlashExposureTime * 2) != FMath::FloorToInt(PrevTime * 2))
		{
			FC_LOG_NET(LogFCNet, Log, TEXT("[%s] ★ Flash 노출 중 - 시간: %.1f / %.1f초"),
				*GetName(), FlashExposureTime, FlashExposureThreshold);
		}
	}
	else
	{
		// 빛에 노출되지 않으면 누적 시간 감소 (초당 0.5초씩 감소)
		FlashExposureTime = FMath::Max(0.0f, FlashExposureTime - (DeltaTime * 0.5f));

		// [디버그] 노출 시간 감소 로그 (시간이 있을 때만)
		if (PrevTime > 0.0f && FlashExposureTime <= 0.0f)
		{
			FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Flash 노출 시간 리셋됨 (빛에서 벗어남)"), *GetName());
		}
	}
}

bool AFCMonsterBlinkHunter::ShouldApplyFlashStun() const
{
	return FlashExposureTime >= FlashExposureThreshold;
}

void AFCMonsterBlinkHunter::ApplyFlashStun()
{
	FC_LOG_NET(LogFCNet, Warning, TEXT("[%s] ★★★ FLASH STUN 적용! 지속시간: %.1f초 ★★★"), *GetName(), FlashStunDuration);

	ApplyStun(FlashStunDuration);
	FlashExposureTime = 0.0f; // 누적 시간 초기화
}

bool AFCMonsterBlinkHunter::IsExposedToSpotLight(USpotLightComponent* SpotLight)
{
	if (!SpotLight) return false;

	// [멀티플레이] Owner 플레이어의 시선 방향 사용 (SpotLight 회전은 복제 안 됨)
	AActor* FlashLightActor = SpotLight->GetOwner();
	if (!FlashLightActor) return false;

	AFCPlayerCharacter* OwnerPlayer = Cast<AFCPlayerCharacter>(FlashLightActor->GetOwner());
	if (!OwnerPlayer) return false;

	// 플레이어 눈 위치와 시선 방향 가져오기
	FVector LightLocation;
	FRotator ViewRotation;
	OwnerPlayer->GetActorEyesViewPoint(LightLocation, ViewRotation);
	const FVector LightForward = ViewRotation.Vector();

	// 공통 함수로 체크 (FlashLight 액터를 무시 대상으로)
	return CheckConeLineOfSight(
		LightLocation,
		LightForward,
		SpotLight->AttenuationRadius,
		SpotLight->OuterConeAngle,
		FlashLightActor
	);
}