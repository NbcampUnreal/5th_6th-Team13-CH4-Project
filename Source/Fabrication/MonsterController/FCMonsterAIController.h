// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FCMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class FABRICATION_API AFCMonsterAIController : public AAIController
{
	GENERATED_BODY()
public:
	AFCMonsterAIController();
protected:
	// 빙의(Spawn) 되었을 때 실행
	virtual void OnPossess(APawn* InPawn) override;

	// 팀 ID 반환 (Perception 시스템이 적/아군 구분할 때 사용)
	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	// 몬스터 팀 ID (예: 2번)
	FGenericTeamId MonsterTeamID;
};
