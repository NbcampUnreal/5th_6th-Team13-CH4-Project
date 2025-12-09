// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterController/FCMonsterAIController.h"
#include "Monster/FCMonsterBase.h"

AFCMonsterAIController::AFCMonsterAIController()
{
	// 팀 ID 설정 (0: No Team, 1: Survivor, 2: Monster 라고 가정)
	MonsterTeamID = FGenericTeamId(2);

	// StateTree AI 컴포넌트 생성 (MoveTo 등 빌트인 AI Task 사용 가능)
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
}

AFCMonsterBase* AFCMonsterAIController::GetMonster() const
{
	return Cast<AFCMonsterBase>(GetPawn());
}

void AFCMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// StateTree AI 로직 시작
	if (StateTreeAIComponent)
	{
		StateTreeAIComponent->StartLogic();
	}
}

void AFCMonsterAIController::OnUnPossess()
{
	// StateTree AI 로직 정지
	if (StateTreeAIComponent)
	{
		StateTreeAIComponent->StopLogic(TEXT("UnPossess"));
	}

	Super::OnUnPossess();
}

FGenericTeamId AFCMonsterAIController::GetGenericTeamId() const
{
	return MonsterTeamID;
}
