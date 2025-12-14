// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/FCMonsterBlinkHunter.h"
#include "Player/FCPlayerCharacter.h"
#include "Flash/FlashLight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

AFCMonsterBlinkHunter::AFCMonsterBlinkHunter()
{
	// 기본 상태 초기화
	bIsBeingWatched = false;
	FlashExposureTime = 0.0f;

	// 기본 이동 속도를 Unseen 속도로 설정
	MoveSpeed_Normal = MoveSpeed_Unseen;
}

void AFCMonsterBlinkHunter::BeginPlay()
{
	Super::BeginPlay();
}

void AFCMonsterBlinkHunter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 관찰 상태 복제 (클라이언트 애니메이션용)
	DOREPLIFETIME(AFCMonsterBlinkHunter, bIsBeingWatched);
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

	// Hit가 없으면 (장애물 없음) true, Hit가 있으면 false
	return !bHit;
}

void AFCMonsterBlinkHunter::CheckFlashLightExposure(float DeltaTime)
{
	// 모든 FlashLight 액터 가져오기
	TArray<AActor*> FlashLightActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlashLight::StaticClass(), FlashLightActors);

	bool bExposedThisFrame = false;

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
			// SpotLight가 켜져 있고, 이 몬스터를 비추고 있는지 체크
			if (SpotLight && SpotLight->IsVisible() && IsExposedToSpotLight(SpotLight))
			{
				bExposedThisFrame = true;
				break;
			}
		}

		if (bExposedThisFrame) break;
	}

	// Flash 빛에 노출되었으면 시간 누적
	if (bExposedThisFrame)
	{
		FlashExposureTime += DeltaTime;

		// 임계값 도달 시 스턴 적용
		if (FlashExposureTime >= FlashExposureThreshold)
		{
			ApplyStun(FlashStunDuration);
			FlashExposureTime = 0.0f; // 누적 시간 초기화
		}
	}
	else
	{
		// 빛에 노출되지 않으면 누적 시간 감소 (예: 초당 0.5초씩 감소)
		FlashExposureTime = FMath::Max(0.0f, FlashExposureTime - (DeltaTime * 0.5f));
	}
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
		return false;
	}

	// 2. SpotLight의 원뿔 각도 내에 있는지 체크
	float DotProduct = FVector::DotProduct(LightForward, ToMonster);
	float OuterConeAngle = SpotLight->OuterConeAngle;
	float AngleCosine = FMath::Cos(FMath::DegreesToRadians(OuterConeAngle));

	if (DotProduct < AngleCosine)
	{
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

	// Hit가 없으면 (장애물 없음) true
	return !bHit;
}