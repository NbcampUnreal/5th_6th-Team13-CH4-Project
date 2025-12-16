// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BlinkHunter/Service/BTS_CheckPlayerGaze.h"
#include "Monster/FCMonsterBlinkHunter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CheckPlayerGaze::UBTS_CheckPlayerGaze()
{
	NodeName = "Check Player Gaze";
	Interval = 0.1f; // 0.1초마다 체크
	RandomDeviation = 0.0f;
}

void UBTS_CheckPlayerGaze::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return;
	}

	// BlinkHunter 획득
	AFCMonsterBlinkHunter* Monster = Cast<AFCMonsterBlinkHunter>(AICon->GetMonster());
	if (!Monster)
	{
		return;
	}

	// 스턴 상태면 체크 안 함
	if (Monster->bIsStunned)
	{
		return;
	}

	// 플레이어 시선 체크
	bool bWatched = Monster->IsBeingWatchedByPlayers();

	// 1. Blackboard 업데이트 (BehaviorTree용)
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(TEXT("IsBeingWatched"), bWatched);
	}

	// 2. Monster의 Replicated 변수 업데이트 (클라이언트 애니메이션용)
	Monster->bIsBeingWatched = bWatched;

	// 3. 이동 속도 조절
	if (bWatched)
	{
		Monster->SetMovementSpeed(Monster->MoveSpeed_Frozen);
	}
	else
	{
		Monster->SetMovementSpeed(Monster->MoveSpeed_Unseen);
	}
}