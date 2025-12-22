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
	// 모든 플레이어 캐릭터 가져오기
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFCPlayerCharacter::StaticClass(), PlayerActors);

	// 한 명이라도 바라보고 있으면 true
	for (AActor* Actor : PlayerActors)
	{
		AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
		if (Player && IsPlayerLookingAtMe(Player))
		{
			return true;
		}
	}

	return false;
}

bool AFCMonsterBlinkHunter::IsPlayerLookingAtMe(AFCPlayerCharacter* Player)
{
	if (!Player) return false;

	// 1. 거리 체크
	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if (Distance > SightCheckDistance)
	{
		return false; // 너무 멀면 볼 수 없음
	}

	// 2. 플레이어의 시선 방향 가져오기
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	Player->GetActorEyesViewPoint(PlayerViewLocation, PlayerViewRotation);

	FVector PlayerForward = PlayerViewRotation.Vector();
	FVector ToMonster = (GetActorLocation() - PlayerViewLocation).GetSafeNormal();

	// 3. Dot Product로 시야각 체크
	float DotProduct = FVector::DotProduct(PlayerForward, ToMonster);
	float AngleCosine = FMath::Cos(FMath::DegreesToRadians(PlayerViewAngle));

	// 디버그: 시선 체크 정보 출력
	UE_LOG(LogTemp, Log, TEXT("[BlinkHunter] Player: %s, Distance: %.0f, DotProduct: %.3f, AngleCosine: %.3f, Looking: %s"),
		*Player->GetName(), Distance, DotProduct, AngleCosine,
		(DotProduct >= AngleCosine) ? TEXT("YES") : TEXT("NO"));
	UE_LOG(LogTemp, Log, TEXT("[BlinkHunter] PlayerViewRotation: %s, PlayerForward: %s"),
		*PlayerViewRotation.ToString(), *PlayerForward.ToString());

	if (DotProduct < AngleCosine)
	{
		return false; // 시야각 밖
	}

	// 4. Line Trace로 장애물 체크 (플레이어와 몬스터 사이에 벽이 있으면 안 보임)
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(Player);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		PlayerViewLocation,
		GetActorLocation(),
		ECC_Visibility,
		CollisionParams
	);

	UE_LOG(LogTemp, Log, TEXT("[BlinkHunter] LineTrace Hit: %s, HitActor: %s"),
		bHit ? TEXT("YES") : TEXT("NO"),
		bHit ? *HitResult.GetActor()->GetName() : TEXT("None"));

	// Hit가 없으면 (장애물 없음) true, Hit가 있으면 false
	return !bHit;
}

bool AFCMonsterBlinkHunter::IsExposedToFlash()
{
	// 모든 FlashLight 액터 가져오기
	TArray<AActor*> FlashLightActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlashLight::StaticClass(), FlashLightActors);

	// [디버그] FlashLight 액터 수 로그
	if (FlashLightActors.Num() > 0)
	{
		FC_LOG_NET(LogFCNet, Verbose, TEXT("[%s] Flash 체크 - FlashLight 액터 수: %d"), *GetName(), FlashLightActors.Num());
	}

	// 각 FlashLight의 SpotLight 컴포넌트 체크
	for (AActor* Actor : FlashLightActors)
	{
		AFlashLight* FlashLight = Cast<AFlashLight>(Actor);
		if (!FlashLight) continue;

		// FlashLight의 SpotLight 컴포넌트 찾기
		TArray<USpotLightComponent*> SpotLights;
		FlashLight->GetComponents<USpotLightComponent>(SpotLights);

		for (USpotLightComponent* SpotLight : SpotLights)
		{
			// [디버그] SpotLight 상태 로그
			if (SpotLight)
			{
				FC_LOG_NET(LogFCNet, Verbose, TEXT("[%s] SpotLight 상태 - Owner: %s, Visible: %s"),
					*GetName(),
					FlashLight->GetOwner() ? *FlashLight->GetOwner()->GetName() : TEXT("None"),
					SpotLight->IsVisible() ? TEXT("ON") : TEXT("OFF"));
			}

			// SpotLight가 켜져 있고, 이 몬스터를 비추고 있는지 체크
			if (SpotLight && SpotLight->IsVisible() && IsExposedToSpotLight(SpotLight))
			{
				FC_LOG_NET(LogFCNet, Log, TEXT("[%s] ★ Flash 빛에 노출됨! FlashLight: %s"), *GetName(), *FlashLight->GetName());
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

	FVector LightLocation = SpotLight->GetComponentLocation();
	FVector LightForward = SpotLight->GetForwardVector();
	FVector ToMonster = (GetActorLocation() - LightLocation).GetSafeNormal();

	// 1. 거리 체크 (SpotLight의 AttenuationRadius 사용)
	float Distance = FVector::Dist(LightLocation, GetActorLocation());
	if (Distance > SpotLight->AttenuationRadius)
	{
		FC_LOG_NET(LogFCNet, Verbose, TEXT("[%s] Flash 거리 실패 - 거리: %.0f, 최대: %.0f"),
			*GetName(), Distance, SpotLight->AttenuationRadius);
		return false;
	}

	// 2. SpotLight의 원뿔 각도 내에 있는지 체크
	float DotProduct = FVector::DotProduct(LightForward, ToMonster);
	float OuterConeAngle = SpotLight->OuterConeAngle;
	float AngleCosine = FMath::Cos(FMath::DegreesToRadians(OuterConeAngle));

	if (DotProduct < AngleCosine)
	{
		FC_LOG_NET(LogFCNet, Verbose, TEXT("[%s] Flash 각도 실패 - DotProduct: %.3f, 필요: %.3f (ConeAngle: %.0f)"),
			*GetName(), DotProduct, AngleCosine, OuterConeAngle);
		return false; // 원뿔 밖
	}

	// 3. Line Trace로 장애물 체크
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(SpotLight->GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		LightLocation,
		GetActorLocation(),
		ECC_Visibility,
		CollisionParams
	);

	if (bHit)
	{
		FC_LOG_NET(LogFCNet, Verbose, TEXT("[%s] Flash 장애물 감지 - HitActor: %s"),
			*GetName(), *HitResult.GetActor()->GetName());
		return false;
	}

	// [디버그] 모든 조건 통과
	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Flash 조건 통과 - 거리: %.0f, DotProduct: %.3f"),
		*GetName(), Distance, DotProduct);

	// Hit가 없으면 (장애물 없음) true
	return true;
}