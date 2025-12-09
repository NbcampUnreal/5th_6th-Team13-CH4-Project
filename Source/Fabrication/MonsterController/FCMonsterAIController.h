// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/StateTreeAIComponent.h"
#include "FCMonsterAIController.generated.h"

/**
 * 몬스터 AI 컨트롤러
 * StateTree AI Component를 통해 행동 트리를 실행합니다.
 */
UCLASS()
class FABRICATION_API AFCMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AFCMonsterAIController();

	// StateTree AI 컴포넌트 접근자
	FORCEINLINE UStateTreeAIComponent* GetStateTreeAIComponent() const { return StateTreeAIComponent; }

	// 소유 중인 몬스터 캐스트 헬퍼
	UFUNCTION(BlueprintCallable, Category = "Monster")
	class AFCMonsterBase* GetMonster() const;

protected:
	// 빙의(Spawn) 되었을 때 실행
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// 팀 ID 반환 (Perception 시스템이 적/아군 구분할 때 사용)
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	// StateTree AI 컴포넌트 (MoveTo 등 AI Task 사용 가능)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;

private:
	// 몬스터 팀 ID (예: 2번)
	FGenericTeamId MonsterTeamID;
};
