// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCClearBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTT_FCClearBlackboardValue::UBTT_FCClearBlackboardValue()
{
	NodeName = "FC Clear Blackboard Value";

	// Task는 즉시 완료됨 (Latent 아님)
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_FCClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 1. Blackboard Component 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// 2. 선택된 키가 유효한지 확인
	if (!BlackboardKey.SelectedKeyName.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	// 3. 블랙보드 값 초기화 (타입에 상관없이 모두 처리)
	BlackboardComp->ClearValue(BlackboardKey.SelectedKeyName);

	return EBTNodeResult::Succeeded;
}