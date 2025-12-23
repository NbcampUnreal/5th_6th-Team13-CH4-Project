// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/FCMonsterBase.h"
#include "FCMonsterBlinkHunter.generated.h"

class UFCFlashDetectionComponent;

/**
 * BlinkHunter 몬스터
 *
 * 핵심 메커니즘:
 * 1. 플레이어가 바라보는 동안 → 움직임 정지 (이동 속도 0)
 * 2. 플레이어가 시선을 돌리면 → 빠르게 접근
 * 3. Flash 빛에 3초 노출 시 → 10초 스턴 (UFCFlashDetectionComponent 사용)
 */
UCLASS()
class FABRICATION_API AFCMonsterBlinkHunter : public AFCMonsterBase
{
	GENERATED_BODY()

#pragma region ACharacter Override

public:
	AFCMonsterBlinkHunter();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** DataTable에서 로드된 데이터 적용 (부모 오버라이드) */
	virtual void ApplyMonsterData() override;

#pragma endregion

#pragma region Configuration (BlinkHunter Stats - DataTable에서 로드)

public:
	/** Flash 빛에 노출되어야 하는 시간 (초, DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float FlashExposureThreshold = 3.0f;

	/** Flash로 인한 스턴 지속 시간 (초, DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float FlashStunDuration = 10.0f;

	/** 플레이어 시야각 (도, DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float PlayerViewAngle = 60.0f;

	/** 시야 체크 최대 거리 (cm, DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float SightCheckDistance = 3000.0f;

	/** 관찰 중일 때 이동 속도 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float MoveSpeed_Frozen = 0.0f;

	/** 관찰받지 않을 때 이동 속도 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float MoveSpeed_Unseen = 800.0f;

#pragma endregion

#pragma region State Data (Replicated)

public:
	/** 현재 플레이어에게 관찰되고 있는지 여부 (클라이언트 애니메이션용) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "BlinkHunter|State")
	bool bIsBeingWatched;

	/** Flash 빛에 노출된 누적 시간 (서버 전용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|State")
	float FlashExposureTime;

	/** Freeze 전 캐싱된 이동 속도 (BT에서 설정한 속도 보존용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|State")
	float CachedMoveSpeed;

	/** 현재 Freeze 상태인지 (속도 복원 판단용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|State")
	bool bIsFrozen;

#pragma endregion

#pragma region Components

protected:
	/** Flash 빛 감지 컴포넌트 (Flash 노출 시간 추적 및 스턴) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlinkHunter|Components")
	TObjectPtr<UFCFlashDetectionComponent> FlashDetectionComp;

#pragma endregion

#pragma region Actor Cache (성능 최적화 - 플레이어 시선 감지용)

protected:
	/** 캐싱된 PlayerCharacter 목록 (WeakPtr로 소멸 감지) */
	TArray<TWeakObjectPtr<class AFCPlayerCharacter>> CachedPlayers;

	/** 마지막 캐시 갱신 시간 */
	float LastCacheUpdateTime = 0.0f;

	/** 캐시 갱신 간격 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "BlinkHunter|Performance")
	float CacheUpdateInterval = 1.0f;

	/** 플레이어 캐시 갱신 */
	void UpdatePlayerCache();

	/** 캐시 갱신이 필요한지 확인 */
	bool ShouldUpdateCache() const;

#pragma endregion

#pragma region Override Functions

public:
	/**
	 * 이동 속도 설정 (오버라이드)
	 * Frozen 상태에서는 캐시만 업데이트하고 실제 속도는 0 유지
	 */
	virtual void SetMovementSpeed(float NewSpeed) override;

#pragma endregion

#pragma region Helper Functions (Service/Task에서 호출)

public:
	/**
	 * Freeze 상태 설정 (Service에서 호출)
	 * Freeze 시 현재 속도를 캐싱하고 0으로 설정
	 * Unfreeze 시 캐싱된 속도로 복원
	 * @param bFreeze true면 정지, false면 복원
	 */
	void SetFrozen(bool bFreeze);

	/**
	 * 플레이어들의 시선을 체크하여 관찰 여부 판정 (Service에서 호출)
	 * @return true면 한 명 이상의 플레이어가 바라보고 있음
	 */
	bool IsBeingWatchedByPlayers();

	/**
	 * Flash 빛에 현재 노출되어 있는지 체크 (Service에서 호출)
	 * @return true면 Flash 빛에 노출됨
	 */
	bool IsExposedToFlash();

	/**
	 * Flash 노출 시간 업데이트 (Service에서 호출)
	 * @param DeltaTime 경과 시간
	 * @param bExposed 현재 노출 여부
	 */
	void UpdateFlashExposureTime(float DeltaTime, bool bExposed);

	/**
	 * Flash 스턴 조건 충족 여부 (Decorator에서 체크)
	 * @return true면 스턴 적용해야 함
	 */
	bool ShouldApplyFlashStun() const;

	/**
	 * Flash 스턴 적용 및 노출 시간 리셋 (Task에서 호출)
	 */
	void ApplyFlashStun();

public:
	/**
	 * 특정 플레이어가 이 몬스터를 바라보고 있는지 체크 (Service에서 호출)
	 * @param Player 체크할 플레이어
	 * @return true면 플레이어가 몬스터를 바라보고 있음
	 */
	bool IsPlayerLookingAtMe(class AFCPlayerCharacter* Player);

protected:
	/**
	 * 원뿔 시야 + 장애물 체크 (IsPlayerLookingAtMe에서 사용)
	 * @param SourceLocation 시선의 시작 위치
	 * @param SourceForward 시선의 방향 벡터
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