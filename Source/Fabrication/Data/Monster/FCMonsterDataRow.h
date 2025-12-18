// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FCMonsterDataRow.generated.h"

/**
 * 몬스터 스탯 DataTable Row
 * - 모든 몬스터 공통 스탯 + 각 몬스터 전용 스탯 포함
 * - RowName으로 몬스터 타입 구분 (예: "SoundHunter", "BlinkHunter")
 * - BeginPlay에서 로드하여 하드코딩 대체
 *
 * [사용법]
 * 1. 에디터에서 DataTable 에셋 생성 (Row Structure: FCMonsterDataRow)
 * 2. 몬스터 블루프린트에서 MonsterDataTable, MonsterRowName 설정
 * 3. BeginPlay에서 자동으로 값 적용
 */
USTRUCT(BlueprintType)
struct FABRICATION_API FFCMonsterDataRow : public FTableRowBase
{
	GENERATED_BODY()

#pragma region Monster Identity (몬스터 식별)

	/**
	 * 몬스터 표시 이름 (UI용)
	 * - RowName이 코드 역할 (예: "MON_SH_001")
	 * - DisplayName은 UI 표시용 (예: "사운드 헌터")
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Identity")
	FText MonsterDisplayName;

#pragma endregion

#pragma region Common - Movement (공통: 이동)

	/** 일반 이동 속도 (순찰, 탐색) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Movement")
	float MoveSpeed_Normal = 400.f;

	/** 추격 이동 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Movement")
	float MoveSpeed_Chasing = 600.f;

#pragma endregion

#pragma region Common - Combat (공통: 전투)

	/** 공격 범위 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Combat")
	float AttackRange = 150.f;

	/** 공격당 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Combat")
	int32 DamagePerAttack = 1;

#pragma endregion

#pragma region Common - Respawn (공통: 리스폰)

	/** 리스폰 쿨타임 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Respawn")
	float RespawnCooldown = 10.0f;

	/** 플레이어로부터 최소 스폰 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Respawn")
	float MinSpawnDistanceFromPlayer = 2000.f;

	/** 스폰 위치 검색 반경 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Respawn")
	float SpawnSearchRadius = 5000.f;

#pragma endregion

#pragma region Common - Perception Sight (공통: 시야)

	/** 시야 감지 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Perception|Sight")
	float SightRadius = 1500.f;

	/** 시야 놓치는 거리 (cm) - SightRadius보다 커야 함 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Perception|Sight")
	float LoseSightRadius = 2000.f;

	/** 시야각 (도, 정면 기준 양쪽) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Perception|Sight")
	float PeripheralVisionAngleDegrees = 90.f;

	/** 시야 활성화 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common|Perception|Sight")
	bool bSightEnabled = true;

#pragma endregion

#pragma region SoundHunter - Hearing (SoundHunter: 청력)

	/** 청력 감지 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundHunter|Hearing")
	float HearingRadius = 3000.f;

	/** 청력 활성화 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundHunter|Hearing")
	bool bHearingEnabled = true;

#pragma endregion

#pragma region SoundHunter - Lure (SoundHunter: 유인)

	/** Lure 지속 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundHunter|Lure")
	float LureDuration = 10.0f;

	/** Lure 도착 판정 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundHunter|Lure")
	float LureArrivalDistance = 200.f;

#pragma endregion

#pragma region BlinkHunter - Flash (BlinkHunter: 플래시)

	/** Flash 노출 임계 시간 (초) - 이 시간 이상 노출 시 스턴 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlinkHunter|Flash")
	float FlashExposureThreshold = 3.0f;

	/** Flash 스턴 지속 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlinkHunter|Flash")
	float FlashStunDuration = 10.0f;

#pragma endregion

#pragma region BlinkHunter - Gaze (BlinkHunter: 시선)

	/** 플레이어 시야각 판정 (도) - 이 각도 내에 있어야 "바라봄" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlinkHunter|Gaze")
	float PlayerViewAngle = 60.f;

	/** 시선 체크 최대 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlinkHunter|Gaze")
	float GazeCheckDistance = 3000.f;

	/** 관찰당할 때 이동 속도 (정지) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlinkHunter|Gaze")
	float MoveSpeed_Frozen = 0.f;

	/** 관찰받지 않을 때 이동 속도 (빠름) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlinkHunter|Gaze")
	float MoveSpeed_Unseen = 800.f;

#pragma endregion
};
