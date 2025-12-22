
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/Monster/FCMonsterDataRow.h"
#include "FCMonsterBase.generated.h"

// 전방 선언
class AFCPlayerCharacter;
class UAnimMontage;
class UDataTable;

UCLASS()
class FABRICATION_API AFCMonsterBase : public ACharacter
{
	GENERATED_BODY()

#pragma region ACharacter Override

public:
	AFCMonsterBase();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma endregion

#pragma region DataTable Configuration

public:
	/** 몬스터 스탯 DataTable (에디터에서 할당) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|DataTable")
	TObjectPtr<UDataTable> MonsterDataTable;

	/** DataTable에서 사용할 Row 이름 (예: "SoundHunter", "BlinkHunter") */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|DataTable")
	FName MonsterRowName;

	/** 캐싱된 몬스터 데이터 (BeginPlay에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|DataTable")
	FFCMonsterDataRow CachedMonsterData;

	/** DataTable 로드 성공 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|DataTable")
	bool bDataTableLoaded = false;

protected:
	/** DataTable에서 몬스터 데이터 로드 (BeginPlay에서 호출) */
	virtual void LoadMonsterDataFromTable();

	/** 로드된 데이터를 실제 스탯에 적용 (자식 클래스에서 오버라이드) */
	virtual void ApplyMonsterData();

public:
	/**
	 * 블루프린트에서 코드(RowName)로 몬스터 데이터 검색
	 * @param DataTable 검색할 DataTable
	 * @param MonsterCode 몬스터 코드 (RowName, 예: "MON_SH_001")
	 * @param bFound 검색 성공 여부
	 * @return 찾은 몬스터 데이터 (실패 시 기본값)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Monster|DataTable", meta = (DisplayName = "Find Monster Data By Code"))
	static FFCMonsterDataRow FindMonsterDataByCode(UDataTable* DataTable, FName MonsterCode, bool& bFound);

	/**
	 * 블루프린트에서 몬스터 표시 이름 가져오기
	 * @param DataTable 검색할 DataTable
	 * @param MonsterCode 몬스터 코드 (RowName)
	 * @return 표시 이름 (실패 시 빈 텍스트)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Monster|DataTable", meta = (DisplayName = "Get Monster Display Name"))
	static FText GetMonsterDisplayName(UDataTable* DataTable, FName MonsterCode);

#pragma endregion

#pragma region Configuration (Stats) - DataTable에서 로드됨

public:
	/** 일반 이동 속도 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Stats|Movement")
	float MoveSpeed_Normal = 400.f;

	/** 추격 이동 속도 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Stats|Movement")
	float MoveSpeed_Chasing = 600.f;

	/** 공격 범위 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Stats|Combat")
	float AttackRange = 150.f;

	/** 공격당 데미지 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Stats|Combat")
	int32 DamagePerAttack = 1;

#pragma endregion

#pragma region AI State Data (BehaviorTree Blackboard Variables)

public:
	/** 현재 추적 중인 타겟 (BehaviorTree 블랙보드 변수) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Monster|AI_State")
	TObjectPtr<AFCPlayerCharacter> TargetPlayer;

	/** 스턴 상태 여부 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Monster|AI_State")
	bool bIsStunned;

	/** 공격 가능 여부 (쿨타임 중이면 false) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Monster|AI_State")
	bool bCanAttack;

#pragma endregion

#pragma region Helper Functions (Task에서 호출)

public:
	/** 이동 속도 변경 (Task: SetSpeed) */
	UFUNCTION(BlueprintCallable, Category = "Monster|Helper")
	virtual void SetMovementSpeed(float NewSpeed);

public:
	// [멀티플레이] 현재 "눈"으로 보고 있는 타겟 (AIController의 Perception에서 업데이트됨)
	// 클라이언트에서 애니메이션/이펙트용으로 사용
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Monster|AI_State")
	TObjectPtr<AFCPlayerCharacter> SeenPlayer;

	// [멀티플레이] 마지막으로 감지된 위치 (AIController의 Perception에서 업데이트됨)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Monster|AI_State")
	FVector LastStimulusLocation;

#pragma endregion

#pragma region Combat Logic

public:
	/** 공격 애니메이션 몬타주 (에디터에서 설정) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	/** 수색 애니메이션 몬타주 (에디터에서 설정) - 플레이어 놓친 후 주변 둘러보기 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Combat")
	TObjectPtr<UAnimMontage> InvestigateMontage;

	/** 스턴 애니메이션 몬타주 (에디터에서 설정) - 스턴 상태일 때 재생 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Combat")
	TObjectPtr<UAnimMontage> StunMontage;

	/** 외부(아이템 등)에서 스턴을 걸 때 호출 */
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	void ApplyStun(float Duration);

	/**
	 * 구 콜리전 기반 근접 공격 수행 (서버 전용)
	 * 몬스터 위치에서 AttackRange 반경 내 플레이어에게 데미지 적용
	 * @return 한 명 이상 명중 시 true
	 */
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	bool PerformMeleeAttack();

	/** 공격 애니메이션 재생 (멀티캐스트) - Task에서 호출 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayAttackAnim();

	/** 수색 애니메이션 재생 (멀티캐스트) - Task에서 호출 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayInvestigateAnim();

	/** 스턴 애니메이션 재생 (멀티캐스트) - Task에서 호출 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayStunAnim();

protected:
	/** 스턴 종료 콜백 */
	void EndStun();

private:
	FTimerHandle StunTimerHandle;

#pragma endregion

#pragma region Respawn Configuration (DataTable에서 로드)

public:
	/** Respawn 쿨타임 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Respawn")
	float RespawnCooldown = 10.0f;

	/** 플레이어로부터 최소 거리 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Respawn")
	float MinSpawnDistanceFromPlayer = 2000.0f;

	/** Respawn 위치 검색 반경 (DataTable에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Respawn")
	float SpawnSearchRadius = 5000.0f;

#pragma endregion

};