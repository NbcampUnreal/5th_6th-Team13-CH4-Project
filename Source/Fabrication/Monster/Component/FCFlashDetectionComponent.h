// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FCFlashDetectionComponent.generated.h"

class AFlashLight;
class AFCPlayerCharacter;
class USpotLightComponent;

/**
 * Flash 빛 감지 컴포넌트
 *
 * BlinkHunter에서 사용되며, Flash 빛에 노출되었는지 감지하고
 * 노출 시간을 누적하여 스턴 조건을 체크합니다.
 *
 * 주요 기능:
 * - FlashLight 액터 캐싱 (성능 최적화)
 * - Flash 빛 노출 여부 체크
 * - 노출 시간 누적 및 감소
 * - 스턴 조건 충족 여부 판정
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FABRICATION_API UFCFlashDetectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFCFlashDetectionComponent();

protected:
	virtual void BeginPlay() override;

#pragma region Configuration

public:
	/** Flash 빛에 노출되어야 하는 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Detection|Config")
	float ExposureThreshold = 3.0f;

	/** Flash로 인한 스턴 지속 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Detection|Config")
	float StunDuration = 10.0f;

	/** 캐시 갱신 간격 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Detection|Config")
	float CacheUpdateInterval = 1.0f;

	/** 노출 시간 감소 비율 (빛에서 벗어났을 때) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Detection|Config")
	float ExposureDecayRate = 0.5f;

#pragma endregion

#pragma region State

public:
	/** 현재 누적된 Flash 노출 시간 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flash Detection|State")
	float ExposureTime = 0.0f;

#pragma endregion

#pragma region Public Interface

public:
	/**
	 * Flash 빛에 현재 노출되어 있는지 체크
	 * @return true면 Flash 빛에 노출됨
	 */
	UFUNCTION(BlueprintCallable, Category = "Flash Detection")
	bool IsExposedToFlash();

	/**
	 * Flash 노출 시간 업데이트
	 * @param DeltaTime 경과 시간
	 * @param bExposed 현재 노출 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Flash Detection")
	void UpdateExposureTime(float DeltaTime, bool bExposed);

	/**
	 * Flash 스턴 조건 충족 여부
	 * @return true면 스턴 적용해야 함
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flash Detection")
	bool ShouldApplyStun() const;

	/**
	 * 노출 시간 리셋 (스턴 적용 후 호출)
	 */
	UFUNCTION(BlueprintCallable, Category = "Flash Detection")
	void ResetExposureTime();

	/**
	 * FlashLight 캐시 강제 갱신
	 */
	UFUNCTION(BlueprintCallable, Category = "Flash Detection")
	void RefreshCache();

#pragma endregion

#pragma region Cache

protected:
	/** 캐싱된 FlashLight 액터 목록 */
	TArray<TWeakObjectPtr<AFlashLight>> CachedFlashLights;

	/** 마지막 캐시 갱신 시간 */
	float LastCacheUpdateTime = 0.0f;

	/** 캐시 갱신이 필요한지 확인 */
	bool ShouldUpdateCache() const;

	/** FlashLight 캐시 갱신 */
	void UpdateCache();

#pragma endregion

#pragma region Internal

protected:
	/**
	 * SpotLight가 소유자를 비추고 있는지 체크
	 * @param SpotLight 체크할 스포트라이트
	 * @return true면 빛에 노출됨
	 */
	bool IsExposedToSpotLight(USpotLightComponent* SpotLight);

	/**
	 * 원뿔 시야 + 장애물 체크
	 * @param SourceLocation 빛의 시작 위치
	 * @param SourceForward 빛의 방향 벡터
	 * @param MaxDistance 최대 감지 거리
	 * @param ConeAngleDegrees 원뿔 각도 (도 단위)
	 * @param IgnoredActor LineTrace에서 무시할 액터
	 * @return true면 시야 내에 있고 장애물 없음
	 */
	bool CheckConeLineOfSight(
		const FVector& SourceLocation,
		const FVector& SourceForward,
		float MaxDistance,
		float ConeAngleDegrees,
		AActor* IgnoredActor
	) const;

#pragma endregion
};
