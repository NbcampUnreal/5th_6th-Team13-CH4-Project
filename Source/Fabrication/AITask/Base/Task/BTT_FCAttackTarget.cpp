// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCAttackTarget.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

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

	// Blackboard에서 TargetPlayer 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AFCPlayerCharacter* TargetPlayer = Cast<AFCPlayerCharacter>(
		BlackboardComp->GetValueAsObject(TEXT("TargetPlayer"))
	);

	// 타겟 유효성 체크 + 공격 가능 여부 체크
	// [버그 수정] bCanAttack 체크 추가 - 쿨타임 중에는 공격 불가
	if (!TargetPlayer || Monster->bIsStunned || !Monster->bCanAttack)
	{
		return EBTNodeResult::Failed;
	}

	// 거리 체크
	float Distance = FVector::Dist(Monster->GetActorLocation(), TargetPlayer->GetActorLocation());
	if (Distance > Monster->AttackRange)
	{
		return EBTNodeResult::Failed; // 사거리 밖
	}

	// 공격 실행
	// 1. 데미지 적용
	UGameplayStatics::ApplyDamage(
		TargetPlayer,
		Monster->DamagePerAttack,
		AICon,
		Monster,
		UDamageType::StaticClass()
	);

	// 2. 공격 애니메이션 재생 (멀티캐스트)
	Monster->Multicast_PlayAttackAnim();

	// 3. 공격 성공 후 bCanAttack = false (Respawn에서 복구)
	Monster->bCanAttack = false;

	return EBTNodeResult::Succeeded;
}