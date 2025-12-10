// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterController/FCMonsterAIController.h"
#include "Monster/FCMonsterBase.h"
#include "Components/StateTreeComponent.h"

AFCMonsterAIController::AFCMonsterAIController()
{
	// 팀 ID 설정 (0: No Team, 1: Survivor, 2: Monster 라고 가정)
	MonsterTeamID = FGenericTeamId(2);

	// StateTreeComponent는 Monster(Character)에 붙이므로 여기서는 생성하지 않음
}

AFCMonsterBase* AFCMonsterAIController::GetMonster() const
{
	return Cast<AFCMonsterBase>(GetPawn());
}

UStateTreeComponent* AFCMonsterAIController::GetStateTreeComponent() const
{
	if (AFCMonsterBase* Monster = GetMonster())
	{
		return Monster->FindComponentByClass<UStateTreeComponent>();
	}
	return nullptr;
}

void AFCMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// StateTreeComponent는 Monster(Character)에 있으므로 거기서 찾아서 시작
	if (UStateTreeComponent* StateTreeComp = GetStateTreeComponent())
	{
		StateTreeComp->StartLogic();
	}
}

void AFCMonsterAIController::OnUnPossess()
{
	// StateTreeComponent는 Monster(Character)에 있으므로 거기서 찾아서 정지
	if (UStateTreeComponent* StateTreeComp = GetStateTreeComponent())
	{
		StateTreeComp->StopLogic(TEXT("UnPossess"));
	}

	Super::OnUnPossess();
}

FGenericTeamId AFCMonsterAIController::GetGenericTeamId() const
{
	return MonsterTeamID;
}
