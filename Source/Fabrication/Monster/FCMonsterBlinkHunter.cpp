// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/FCMonsterBlinkHunter.h"
#include "Monster/Component/FCFlashDetectionComponent.h"
#include "Player/FCPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Fabrication.h"

AFCMonsterBlinkHunter::AFCMonsterBlinkHunter()
{
	// Flash 감지 컴포넌트 생성
	FlashDetectionComp = CreateDefaultSubobject<UFCFlashDetectionComponent>(TEXT("FlashDetectionComp"));

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

	// 초기 플레이어 캐시 갱신 (Flash 캐시는 컴포넌트에서 관리)
	UpdatePlayerCache();
}

bool AFCMonsterBlinkHunter::ShouldUpdateCache() const
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	return (CurrentTime - LastCacheUpdateTime) >= CacheUpdateInterval;
}

void AFCMonsterBlinkHunter::UpdatePlayerCache()
{
	// 플레이어 캐시 초기화 (FlashLight 캐시는 FlashDetectionComp에서 관리)
	CachedPlayers.Empty();

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

	// FlashDetectionComp에도 동기화
	if (FlashDetectionComp)
	{
		FlashDetectionComp->ExposureThreshold = FlashExposureThreshold;
		FlashDetectionComp->StunDuration = FlashStunDuration;
	}

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
		UpdatePlayerCache();
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
	// FlashDetectionComp에 위임
	if (FlashDetectionComp)
	{
		return FlashDetectionComp->IsExposedToFlash();
	}
	return false;
}

void AFCMonsterBlinkHunter::UpdateFlashExposureTime(float DeltaTime, bool bExposed)
{
	// FlashDetectionComp에 위임
	if (FlashDetectionComp)
	{
		FlashDetectionComp->UpdateExposureTime(DeltaTime, bExposed);
		// 로컬 변수도 동기화 (Replicated 또는 UI 표시용)
		FlashExposureTime = FlashDetectionComp->ExposureTime;
	}
}

bool AFCMonsterBlinkHunter::ShouldApplyFlashStun() const
{
	// FlashDetectionComp에 위임
	if (FlashDetectionComp)
	{
		return FlashDetectionComp->ShouldApplyStun();
	}
	return false;
}

void AFCMonsterBlinkHunter::ApplyFlashStun()
{
	FC_LOG_NET(LogFCNet, Warning, TEXT("[%s] ★★★ FLASH STUN 적용! 지속시간: %.1f초 ★★★"), *GetName(), FlashStunDuration);

	ApplyStun(FlashStunDuration);

	// 컴포넌트와 로컬 변수 모두 리셋
	if (FlashDetectionComp)
	{
		FlashDetectionComp->ResetExposureTime();
	}
	FlashExposureTime = 0.0f;
}

