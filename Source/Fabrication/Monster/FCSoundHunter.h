// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/FCMonsterBase.h"
#include "FCSoundHunter.generated.h"

/**
 * SoundHunter 몬스터
 * - 소리로 플레이어를 추적하는 몬스터
 * - 유인 아이템(알람시계, 소리 인형)에 반응하여 해당 위치로 이동
 * - Hearing Sense가 Sight보다 우선순위 높음
 */
UCLASS()
class FABRICATION_API AFCSoundHunter : public AFCMonsterBase
{
	GENERATED_BODY()

public:
	AFCSoundHunter();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** DataTable에서 로드된 데이터 적용 (부모 오버라이드) */
	virtual void ApplyMonsterData() override;

#pragma region SoundHunter Configuration (DataTable에서 로드)

public:
	/** 청력 범위 (DataTable에서 로드, AIController에서 참조) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SoundHunter|Stats")
	float HearingRange = 3000.0f;

	/** Lure(유인) 지속 시간 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SoundHunter|Stats")
	float LureDuration = 10.0f;

	/** Lure 도착 판정 거리 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SoundHunter|Stats")
	float LureArrivalDistance = 200.0f;

#pragma endregion

#pragma region Lure State (Replicated)

public:
	/** [멀티플레이] 현재 Lure(유인) 타겟이 있는지 여부 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "SoundHunter|AI_State")
	bool bHasLureTarget;

	/** [멀티플레이] Lure 위치 (알람시계, 소리 인형 등) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "SoundHunter|AI_State")
	FVector LureLocation;

	/** [멀티플레이] 마지막으로 소리를 들은 위치 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "SoundHunter|AI_State")
	FVector LastHeardLocation;

	/** [멀티플레이] 소리를 들었는지 여부 (일반 소리용) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "SoundHunter|AI_State")
	bool bHasHeardSound;

#pragma endregion

#pragma region Lure Helper Functions

public:
	/** Lure 타겟 설정 (AIController에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "SoundHunter|Lure")
	void SetLureTarget(const FVector& Location);

	/** Lure 타겟 해제 */
	UFUNCTION(BlueprintCallable, Category = "SoundHunter|Lure")
	void ClearLureTarget();

	/** Lure 위치에 도착했는지 확인 */
	UFUNCTION(BlueprintCallable, Category = "SoundHunter|Lure")
	bool HasArrivedAtLure() const;

#pragma endregion

#pragma region Heard Sound Helper Functions

public:
	/** 소리 들음 상태 설정 (AIController에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "SoundHunter|Sound")
	void SetHeardSound(const FVector& Location);

	/** 소리 들음 상태 해제 */
	UFUNCTION(BlueprintCallable, Category = "SoundHunter|Sound")
	void ClearHeardSound();

	/** 소리 위치에 도착했는지 확인 */
	UFUNCTION(BlueprintCallable, Category = "SoundHunter|Sound")
	bool HasArrivedAtHeardLocation() const;

#pragma endregion
};