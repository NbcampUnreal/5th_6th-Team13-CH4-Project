
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FCMonsterBase.generated.h"

// 전방 선언
class AFCPlayerCharacter;
class UAnimMontage;

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

#pragma region Configuration (Stats)

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Movement")
	float MoveSpeed_Normal = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Movement")
	float MoveSpeed_Chasing = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Combat")
	float AttackRange = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Combat")
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
	void SetMovementSpeed(float NewSpeed);

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

	/** 외부(아이템 등)에서 스턴을 걸 때 호출 */
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	void ApplyStun(float Duration);

	/** 공격 애니메이션 재생 (멀티캐스트) - Task에서 호출 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayAttackAnim();

	/** 수색 애니메이션 재생 (멀티캐스트) - Task에서 호출 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayInvestigateAnim();

protected:
	/** 스턴 종료 콜백 */
	void EndStun();

private:
	FTimerHandle StunTimerHandle;

#pragma endregion

#pragma region Respawn Configuration (Task에서 사용)

public:
	/** Respawn 쿨타임 (공격 후 재등장까지 대기 시간) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Respawn")
	float RespawnCooldown = 10.0f;

	/** 플레이어로부터 최소 거리 (Respawn 위치 검색 시) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Respawn")
	float MinSpawnDistanceFromPlayer = 2000.0f;

	/** Respawn 위치 검색 반경 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Respawn")
	float SpawnSearchRadius = 5000.0f;

#pragma endregion

};