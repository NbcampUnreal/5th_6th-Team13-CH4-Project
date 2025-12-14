
// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BTT_FCGetInvestigateLocation.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTT_FCGetInvestigateLocation::UBTT_FCGetInvestigateLocation()
{
	NodeName = "FC Get Investigate Location";

	// 블랙보드 키 필터 설정 (Vector 타입만 선택 가능)
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCGetInvestigateLocation, TargetLocationKey));
}

EBTNodeResult::Type UBTT_FCGetInvestigateLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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

	// 타겟이 없으면 실패
	if (!TargetPlayer)
	{
		return EBTNodeResult::Failed;
	}

	// NavigationSystem 획득
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Monster->GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	// 몬스터와 플레이어 사이의 방향 벡터 계산
	FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - Monster->GetActorLocation()).GetSafeNormal();

	// 그 방향으로 일정 거리(800cm) 앞의 지점을 기준점으로 설정
	FVector SearchOrigin = Monster->GetActorLocation() + (DirectionToPlayer * 800.0f);

	// 기준점 근처의 네비게이션 가능한 랜덤 위치 찾기
	FNavLocation NavLoc;
	if (NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, 500.0f, NavLoc))
	{
		// Blackboard에 결과 저장
		BlackboardComp->SetValueAsVector(TargetLocationKey.SelectedKeyName, NavLoc.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}