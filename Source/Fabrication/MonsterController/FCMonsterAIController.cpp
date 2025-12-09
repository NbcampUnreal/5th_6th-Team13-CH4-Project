// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterController/FCMonsterAIController.h"

AFCMonsterAIController::AFCMonsterAIController()
{
	// 팀 ID 설정 (0: No Team, 1: Survivor, 2: Monster 라고 가정)
	MonsterTeamID = FGenericTeamId(2);
}

void AFCMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

FGenericTeamId AFCMonsterAIController::GetGenericTeamId() const
{
	return MonsterTeamID;
}
