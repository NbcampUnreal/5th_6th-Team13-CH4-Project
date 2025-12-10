// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/STT_FindRandomLocation.h"
#include "NavigationSystem.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Monster/FCMonsterBase.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_FindRandomLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 1. 인스턴스 데이터(쓰기 권한) 가져오기
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	// 2. Owner는 AIController -> GetMonster()로 Monster 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(Context.GetOwner());
	if (!AICon) return EStateTreeRunStatus::Failed;

	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster) return EStateTreeRunStatus::Failed;

	// 3. Navigation System 획득
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Monster->GetWorld());
	if (!NavSys) return EStateTreeRunStatus::Failed;

	// 4. 현재 위치 기준 랜덤 위치 찾기
	const FVector CurrentLocation = Monster->GetActorLocation();
	FNavLocation ResultLocation;

	// GetRandomReachablePointInRadius: 현재 위치 기준 반경 내에서 도달 가능한 랜덤 위치 찾기
	const bool bFound = NavSys->GetRandomReachablePointInRadius(
		CurrentLocation,
		InstanceData.SearchRadius,
		ResultLocation
	);

	if (bFound)
	{
		// 5. 결과값 저장 (StateTree 변수로 전달됨)
		InstanceData.RandomLocation = ResultLocation.Location;
		return EStateTreeRunStatus::Succeeded;
	}

	// 랜덤 위치 찾기 실패 (Navigation Mesh 없음 등)
	return EStateTreeRunStatus::Failed;
}