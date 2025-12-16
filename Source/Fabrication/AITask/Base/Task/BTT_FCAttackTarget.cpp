// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCAttackTarget.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FCAttackTarget::UBTT_FCAttackTarget()
{
	NodeName = "FC Attack Target";
}

EBTNodeResult::Type UBTT_FCAttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// [멀티플레이] BehaviorTree는 서버에서만 실행되므로 별도의 Authority 체크 불필요

	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	// Monster 획득
	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	// 공격 가능 여부 체크 (스턴, 쿨타임)
	if (Monster->bIsStunned || !Monster->bCanAttack)
	{
		return EBTNodeResult::Failed;
	}

	// 구 콜리전 기반 근접 공격 수행
	// (몬스터 위치에서 AttackRange 반경 내 플레이어에게 데미지 + 애니메이션 재생)
	Monster->PerformMeleeAttack();

	// 공격 후 bCanAttack = false (쿨타임 시작, Respawn/다른 Task에서 복구)
	Monster->bCanAttack = false;

	return EBTNodeResult::Succeeded;
}