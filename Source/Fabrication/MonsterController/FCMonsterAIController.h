// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FCMonsterAIController.generated.h"

/**
 * 몬스터 AI 컨트롤러 (StateTreeComponent 사용)
 * StateTreeComponent는 Monster(Character)에 붙이고, 이 컨트롤러가 시작/정지만 담당합니다.
 *
 * [블루프린트 설정 방법]
 * 1. BP_MonsterBase에 StateTreeComponent 추가
 * 2. StateTreeComponent의 StateTree 속성에 ST_MonsterTree 할당
 * 3. ST_MonsterTree Schema: StateTreeComponentSchema 사용
 * 4. ST_MonsterTree Context Actor: BP_MonsterBase (또는 Self)
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

	// StateTreeComponent 접근자 (Monster에서 찾아옴)
	UFUNCTION(BlueprintCallable, Category = "Monster")
	class UStateTreeComponent* GetStateTreeComponent() const;

protected:
	// 빙의(Spawn) 되었을 때 실행
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// 팀 ID 반환 (Perception 시스템이 적/아군 구분할 때 사용)
	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	// 몬스터 팀 ID (예: 2번)
	FGenericTeamId MonsterTeamID;
};
