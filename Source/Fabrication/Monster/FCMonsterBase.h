
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StateTreeComponent.h"
#include "Perception/AIPerceptionTypes.h" // Perception 관련 헤더
#include "FCMonsterBase.generated.h"

// 전방 선언
//class ASurvivorCharacter;

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

#pragma region Components

public:
	/** AI 로직을 수행할 StateTree 컴포넌트 */
	FORCEINLINE UStateTreeComponent* GetStateTreeComponent() const { return StateTreeComponent; }
	
	// AI 감각 시스템 (눈, 귀)
	FORCEINLINE class UAIPerceptionComponent* GetAIPerception() const { return AIPerceptionComponent; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Components")
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Components")
	TObjectPtr<class UAIPerceptionComponent> AIPerceptionComponent;

#pragma endregion

#pragma region Configuration (Stats)

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Movement")
	float MoveSpeed_Normal = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Movement")
	float MoveSpeed_Chasing = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Combat")
	float AttackRange = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Combat")
	float AttackCooldown = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Stats|Combat")
	int32 DamagePerAttack = 1;

#pragma endregion

#pragma region AI State Data (StateTree Variables)

public:
	/** 현재 추적 중인 타겟 (StateTree가 이 변수를 관찰하여 상태 전이) */
	//UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Monster|AI_State")
	//TObjectPtr<ASurvivorCharacter> TargetPlayer;

	/** 스턴 상태 여부 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Monster|AI_State")
	bool bIsStunned;

	/** 공격 가능 여부 (쿨타임 중이면 false) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Monster|AI_State")
	bool bCanAttack;

#pragma endregion

#pragma region AI Actions (StateTree Tasks API)

public:
	/** 이동 속도 변경 (Task: SetSpeed) */
	UFUNCTION(BlueprintCallable, Category = "Monster|AI_Action")
	void SetMovementSpeed(float NewSpeed);

	/** 타겟을 향해 공격 시도 (Task: AttackTarget) -> 성공 시 서버 RPC 호출 */
	UFUNCTION(BlueprintCallable, Category = "Monster|AI_Action")
	bool TryAttackTarget();

	//UFUNCTION(BlueprintCallable, Category = "Monster|AI_Action")
	//ASurvivorCharacter* GetSeenPlayer();

	UFUNCTION(BlueprintCallable, Category = "Monster|AI_Action")
	bool GetInvestigateLocation(FVector& OutLocation);

	// 감각 업데이트 시 호출될 델리게이트 함수
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

protected:
	// 시야 설정(Config) 변수
	UPROPERTY()
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
    
	// 현재 "눈"으로 보고 있는 타겟
	//UPROPERTY(VisibleAnywhere, Category = "Monster|AI_State")
	//TObjectPtr<ASurvivorCharacter> SeenPlayer;

	// 마지막으로 감지된 위치 (놓쳤을 때 마지막으로 가볼 곳)
	UPROPERTY(VisibleAnywhere, Category = "Monster|AI_State")
	FVector LastStimulusLocation;
	
#pragma endregion

#pragma region Combat Logic

public:
	/** 외부(아이템 등)에서 스턴을 걸 때 호출 */
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	void ApplyStun(float Duration);

protected:
	/** 실제 공격 처리 (서버) */
	//UFUNCTION(Server, Reliable)
	//void Server_Attack(ASurvivorCharacter* Target);

	/** 공격 애니메이션 재생 (멀티캐스트) */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayAttackAnim();

	/** 공격 쿨타임 종료 콜백 */
	void ResetAttackCooldown();

	/** 스턴 종료 콜백 */
	void EndStun();

private:
	FTimerHandle AttackTimerHandle;
	FTimerHandle StunTimerHandle;

#pragma endregion

#pragma region Respawn Cycle (Hit & Run)

protected:
	/** 공격 성공 후 몬스터를 숨기고 AI를 정지 (사라짐 연출) */
	void Vanish();

	/** 쿨타임 후, 적절한 위치를 찾아 다시 등장 */
	void Respawn();

	/** 플레이어와 일정 거리 이상 떨어진 안전한 네비게이션 위치 검색 */
	bool GetRandomSpawnLocation(FVector& OutLocation);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster|Respawn")
	float RespawnCooldown = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Monster|Respawn")
	float MinSpawnDistanceFromPlayer = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Monster|Respawn")
	float SpawnSearchRadius = 5000.0f;

private:
	FTimerHandle RespawnTimerHandle;

#pragma endregion

};