// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/Component/FCFlashDetectionComponent.h"
#include "Flash/FlashLight.h"
#include "Player/FCPlayerCharacter.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Fabrication.h"

UFCFlashDetectionComponent::UFCFlashDetectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFCFlashDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 초기 캐시 갱신
	UpdateCache();
}

bool UFCFlashDetectionComponent::ShouldUpdateCache() const
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	return (CurrentTime - LastCacheUpdateTime) >= CacheUpdateInterval;
}

void UFCFlashDetectionComponent::UpdateCache()
{
	CachedFlashLights.Empty();

	TArray<AActor*> FlashLightActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlashLight::StaticClass(), FlashLightActors);

	for (AActor* Actor : FlashLightActors)
	{
		if (AFlashLight* FlashLight = Cast<AFlashLight>(Actor))
		{
			CachedFlashLights.Add(FlashLight);
		}
	}

	LastCacheUpdateTime = GetWorld()->GetTimeSeconds();
}

void UFCFlashDetectionComponent::RefreshCache()
{
	UpdateCache();
}

bool UFCFlashDetectionComponent::IsExposedToFlash()
{
	// 캐시 갱신이 필요하면 갱신
	if (ShouldUpdateCache())
	{
		UpdateCache();
	}

	AActor* Owner = GetOwner();
	if (!Owner) return false;

	// [DEBUG] 캐싱된 FlashLight 개수 확인
	if (CachedFlashLights.Num() == 0)
	{
		FC_LOG_SUBOBJECT_ROLE(LogFCNet, Warning, TEXT("[FlashDetection] CachedFlashLights가 비어있음! UpdateCache 확인 필요"));
	}

	// 캐싱된 FlashLight 목록에서 체크
	for (const TWeakObjectPtr<AFlashLight>& WeakFlashLight : CachedFlashLights)
	{
		AFlashLight* FlashLight = WeakFlashLight.Get();
		if (!FlashLight) continue;

		// Owner 플레이어의 bFlashLightOn 상태로 체크
		AFCPlayerCharacter* OwnerPlayer = Cast<AFCPlayerCharacter>(FlashLight->GetOwner());
		bool bFlashOn = false;
		if (OwnerPlayer)
		{
			bFlashOn = OwnerPlayer->bFlashLightOn;
			FC_LOG_SUBOBJECT_ROLE(LogFCNet, Verbose, TEXT("[FlashDetection] FlashLight Owner: %s, bFlashLightOn: %s"),
				*OwnerPlayer->GetName(), bFlashOn ? TEXT("TRUE") : TEXT("FALSE"));
		}
		else
		{
			// [DEBUG] Owner가 없는 경우
			AActor* RawOwner = FlashLight->GetOwner();
			FC_LOG_SUBOBJECT_ROLE(LogFCNet, Warning, TEXT("[FlashDetection] FlashLight(%s)의 Owner가 Player가 아님! RawOwner: %s"),
				*FlashLight->GetName(), RawOwner ? *RawOwner->GetName() : TEXT("NULL"));
		}

		// FlashLight의 SpotLight 컴포넌트 찾기
		TArray<USpotLightComponent*> SpotLights;
		FlashLight->GetComponents<USpotLightComponent>(SpotLights);

		for (USpotLightComponent* SpotLight : SpotLights)
		{
			if (SpotLight && bFlashOn && IsExposedToSpotLight(SpotLight))
			{
				FC_LOG_SUBOBJECT_ROLE(LogFCNet, Log, TEXT("Flash 빛에 노출됨! FlashLight: %s"), *FlashLight->GetName());
				return true;
			}
		}
	}

	return false;
}

bool UFCFlashDetectionComponent::IsExposedToSpotLight(USpotLightComponent* SpotLight)
{
	if (!SpotLight) return false;

	AActor* FlashLightActor = SpotLight->GetOwner();
	if (!FlashLightActor) return false;

	AFCPlayerCharacter* OwnerPlayer = Cast<AFCPlayerCharacter>(FlashLightActor->GetOwner());
	if (!OwnerPlayer) return false;

	// 플레이어 눈 위치와 시선 방향 가져오기
	FVector LightLocation;
	FRotator ViewRotation;
	OwnerPlayer->GetActorEyesViewPoint(LightLocation, ViewRotation);
	const FVector LightForward = ViewRotation.Vector();

	return CheckConeLineOfSight(
		LightLocation,
		LightForward,
		SpotLight->AttenuationRadius,
		SpotLight->OuterConeAngle,
		FlashLightActor
	);
}

bool UFCFlashDetectionComponent::CheckConeLineOfSight(
	const FVector& SourceLocation,
	const FVector& SourceForward,
	float MaxDistance,
	float ConeAngleDegrees,
	AActor* IgnoredActor
) const
{
	AActor* Owner = GetOwner();
	if (!Owner) return false;

	const FVector OwnerLocation = Owner->GetActorLocation();

	// 1. 거리 체크
	const float Distance = FVector::Dist(SourceLocation, OwnerLocation);
	if (Distance > MaxDistance)
	{
		return false;
	}

	// 2. DotProduct로 원뿔 각도 체크
	const FVector ToOwner = (OwnerLocation - SourceLocation).GetSafeNormal();
	const float DotProduct = FVector::DotProduct(SourceForward, ToOwner);
	const float AngleCosine = FMath::Cos(FMath::DegreesToRadians(ConeAngleDegrees));

	if (DotProduct < AngleCosine)
	{
		return false;
	}

	// 3. LineTrace로 장애물 체크
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Owner);
	if (IgnoredActor)
	{
		CollisionParams.AddIgnoredActor(IgnoredActor);
	}

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		SourceLocation,
		OwnerLocation,
		ECC_Visibility,
		CollisionParams
	);

	return !bHit;
}

void UFCFlashDetectionComponent::UpdateExposureTime(float DeltaTime, bool bExposed)
{
	float PrevTime = ExposureTime;

	if (bExposed)
	{
		ExposureTime += DeltaTime;

		// 0.5초마다 로그
		if (FMath::FloorToInt(ExposureTime * 2) != FMath::FloorToInt(PrevTime * 2))
		{
			FC_LOG_SUBOBJECT_ROLE(LogFCNet, Log, TEXT("★ Flash 노출 중 - 시간: %.1f / %.1f초"),
				ExposureTime, ExposureThreshold);
		}
	}
	else
	{
		ExposureTime = FMath::Max(0.0f, ExposureTime - (DeltaTime * ExposureDecayRate));

		if (PrevTime > 0.0f && ExposureTime <= 0.0f)
		{
			FC_LOG_SUBOBJECT_ROLE(LogFCNet, Log, TEXT("Flash 노출 시간 리셋됨 (빛에서 벗어남)"));
		}
	}
}

bool UFCFlashDetectionComponent::ShouldApplyStun() const
{
	return ExposureTime >= ExposureThreshold;
}

void UFCFlashDetectionComponent::ResetExposureTime()
{
	ExposureTime = 0.0f;
}
