
// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BTT_FCGetInvestigateLocation.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FCGetInvestigateLocation::UBTT_FCGetInvestigateLocation()
{
	NodeName = "FC Get Investigate Location";

	// 블랙보드 키 필터 설정 (Vector 타입만 선택 가능)
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCGetInvestigateLocation, TargetLocationKey));
}

EBTNodeResult::Type UBTT_FCGetInvestigateLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// 수색 위치 계산 (Monster 내부에서 HasAuthority() 체크함)
	FVector ResultLocation;
	if (Monster->GetInvestigateLocation(ResultLocation))
	{
		// 블랙보드에 결과 저장
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocationKey.SelectedKeyName, ResultLocation);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}