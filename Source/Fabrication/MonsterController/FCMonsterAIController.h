// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "FCMonsterAIController.generated.h"

/**
 * 몬스터 AI 컨트롤러 (BehaviorTree 사용)
 * BehaviorTree 에셋을 실행하고 관리합니다.
 * AI의 감각(Perception)도 이곳에서 처리합니다.
 *
 * [블루프린트 설정 방법]
 * 1. BP_MonsterBase의 AIController Class를 BP_MonsterAIController로 설정
 * 2. BP_MonsterAIController의 BehaviorTree 속성에 BT_Monster 할당
 */
UCLASS()
class FABRICATION_API AFCMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AFCMonsterAIController();

	// 소유 중인 몬스터 캐스트 헬퍼
	UFUNCTION(BlueprintCallable, Category = "Monster")
	class AFCMonsterBase* GetMonster() const;

	// BehaviorTree 재시작 (Respawn 시 사용)
	UFUNCTION(BlueprintCallable, Category = "Monster")
	void RestartBehaviorTree();

	// BehaviorTree 정지 (Vanish 시 사용)
	UFUNCTION(BlueprintCallable, Category = "Monster")
	void StopBehaviorTree();

protected:
	// 빙의(Spawn) 되었을 때 실행
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// 팀 ID 반환 (Perception 시스템이 적/아군 구분할 때 사용)
	virtual FGenericTeamId GetGenericTeamId() const override;

	/**
	 * 통합 Perception 핸들러 (서버에서만 실행)
	 * Sense 타입에 따라 HandleSightStimulus / HandleHearingStimulus로 분기
	 */
	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/**
	 * Sight 자극 처리 (자식 클래스에서 오버라이드 가능)
	 * @param Player 감지된 플레이어
	 * @param Stimulus 감각 자극 정보
	 */
	virtual void HandleSightStimulus(class AFCPlayerCharacter* Player, const FAIStimulus& Stimulus);

	/**
	 * Hearing 자극 처리 (자식 클래스에서 오버라이드)
	 * 기본 구현: 아무것도 안 함 (Hearing 미사용 몬스터용)
	 * @param Actor 소리를 낸 액터
	 * @param Stimulus 감각 자극 정보
	 */
	virtual void HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);

	/**
	 * Sight 자극 처리 공통 함수 (자식 클래스에서 재사용 가능)
	 * @param Player 감지된 플레이어
	 * @param Stimulus 감각 자극 정보
	 */
	virtual void HandleSightStimulus(class AFCPlayerCharacter* Player, const FAIStimulus& Stimulus);

protected:
	// [Blackboard Component] : 실제 실행 중 데이터를 저장하는 "기억장치"
	// 실제 값들을 들고 있음 (Key에 해당하는 실시간 값 저장소)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBlackboardComponent> BlackboardComp;

	// [BehaviorTree 에셋] : AI 행동 로직 (블루프린트에서 할당)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	// [AI 감각 시스템] : 눈, 귀 등 (서버에서만 실행)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<class UAIPerceptionComponent> AIPerceptionComponent;

	// [시야 설정] : Sight Config
	UPROPERTY()
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

#pragma region Sight Configuration (Blueprint Editable)

public:
	/** 시야 감지 거리 (cm) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float SightRadius = 1500.0f;

	/** 시야에서 놓치는 거리 (SightRadius보다 커야 함) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float LoseSightRadius = 2000.0f;

	/** 시야각 (도, 정면 기준 양쪽 각도) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float PeripheralVisionAngleDegrees = 90.0f;

	/** 시야 활성화 여부 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Perception|Sight")
	bool bSightEnabled = true;

protected:
	/** OnPossess에서 호출 - Sight 설정 적용 */
	virtual void ApplySightConfig();

#pragma endregion

public:
	// Getter 함수 – 외부에서 BlackboardComp에 접근할 수 있게 해줌
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const
	{
		return BlackboardComp;
	}

private:
	// 몬스터 팀 ID (예: 2번)
	FGenericTeamId MonsterTeamID;
};
