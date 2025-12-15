// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Decorator/BTD_FCInAttackRange.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_FCInAttackRange::UBTD_FCInAttackRange()
{
	NodeName = "Is In Attack Range";
}

bool UBTD_FCInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return false;
	}

	// Monster 획득
	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster)
	{
		return false;
	}

	// Blackboard에서 TargetPlayer 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}

	AFCPlayerCharacter* TargetPlayer = Cast<AFCPlayerCharacter>(
		BlackboardComp->GetValueAsObject(TEXT("TargetPlayer"))
	);

	if (!TargetPlayer)
	{
		return false;
	}

	// 거리 계산
	float Distance = FVector::Dist(Monster->GetActorLocation(), TargetPlayer->GetActorLocation());

	// 공격 범위 내인지 체크
	return Distance <= Monster->AttackRange;
}
