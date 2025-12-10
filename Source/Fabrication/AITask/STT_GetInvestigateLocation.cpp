// Fill out your copyright notice in the Description page of Project Settings.


#include "AITask/STT_GetInvestigateLocation.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_GetInvestigateLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 1. 인스턴스 데이터(쓰기 권한) 가져오기
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	// 2. Owner는 AIController -> GetMonster()로 Monster 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(Context.GetOwner());
	if (!AICon) return EStateTreeRunStatus::Failed;

	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster) return EStateTreeRunStatus::Failed;

	// 3. 수색 위치 계산 함수 호출
	FVector ResultLocation;
	if (Monster->GetInvestigateLocation(ResultLocation))
	{
		// 4. 결과값 저장 (StateTree 변수로 전달됨)
		InstanceData.TargetLocation = ResultLocation;
		return EStateTreeRunStatus::Succeeded;
	}

	// 위치 계산 실패 (타겟 없음 등)
	return EStateTreeRunStatus::Failed;
}