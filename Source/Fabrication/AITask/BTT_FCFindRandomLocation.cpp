// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BTT_FCFindRandomLocation.h"
#include "NavigationSystem.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Monster/FCMonsterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FCFindRandomLocation::UBTT_FCFindRandomLocation()
{
	NodeName = "FC Find Random Location";

	// 블랙보드 키 필터 설정 (Vector 타입만 선택 가능)
	RandomLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCFindRandomLocation, RandomLocationKey));
}

EBTNodeResult::Type UBTT_FCFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// [멀티플레이] BehaviorTree는 서버에서만 실행되므로 별도의 Authority 체크 불필요
	// Blackboard 값은 서버 전용이며 클라이언트로 복제되지 않음

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

	// Navigation System 획득
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Monster->GetWorld());
	if (!NavSys)
	{
		return EBTNodeResult::Failed;
	}

	// 현재 위치 기준 랜덤 위치 찾기
	const FVector CurrentLocation = Monster->GetActorLocation();
	FNavLocation ResultLocation;

	const bool bFound = NavSys->GetRandomReachablePointInRadius(
		CurrentLocation,
		SearchRadius,
		ResultLocation
	);

	if (bFound)
	{
		// 블랙보드에 결과 저장
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(RandomLocationKey.SelectedKeyName, ResultLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}