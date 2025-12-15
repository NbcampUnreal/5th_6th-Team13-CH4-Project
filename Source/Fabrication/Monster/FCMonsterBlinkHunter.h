// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/FCMonsterBase.h"
#include "FCMonsterBlinkHunter.generated.h"

/**
 * BlinkHunter 몬스터
 *
 * 핵심 메커니즘:
 * 1. 플레이어가 바라보는 동안 → 움직임 정지 (이동 속도 0)
 * 2. 플레이어가 시선을 돌리면 → 빠르게 접근
 * 3. Flash 빛에 3초 노출 시 → 10초 스턴
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

#pragma endregion

#pragma region Configuration (BlinkHunter Stats)

public:
	/** Flash 빛에 노출되어야 하는 시간 (초) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float FlashExposureThreshold = 3.0f;

	/** Flash로 인한 스턴 지속 시간 (초) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float FlashStunDuration = 10.0f;

	/** 플레이어 시야각 (도) - 이 각도 내에 있어야 "바라보고 있음"으로 판정 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float PlayerViewAngle = 60.0f;

	/** 시야 체크 최대 거리 (cm) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float SightCheckDistance = 3000.0f;

	/** 관찰 중일 때 이동 속도 (완전 정지) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlinkHunter|Stats")
	float MoveSpeed_Frozen = 0.0f;

	/** 관찰받지 않을 때 이동 속도 (매우 빠름) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlinkHunter|Stats")
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

#pragma endregion

#pragma region Helper Functions (Service/Task에서 호출)

public:
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

protected:
	/**
	 * 특정 플레이어가 이 몬스터를 바라보고 있는지 체크
	 * @param Player 체크할 플레이어
	 * @return true면 플레이어가 몬스터를 바라보고 있음
	 */
	bool IsPlayerLookingAtMe(class AFCPlayerCharacter* Player);

	/**
	 * SpotLight가 이 몬스터를 비추고 있는지 체크
	 * @param SpotLight 체크할 스포트라이트
	 * @return true면 빛에 노출됨
	 */
	bool IsExposedToSpotLight(class USpotLightComponent* SpotLight);

#pragma endregion

};