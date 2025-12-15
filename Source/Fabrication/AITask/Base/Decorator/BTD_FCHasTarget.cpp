// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Decorator/BTD_FCHasTarget.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_FCHasTarget::UBTD_FCHasTarget()
{
	NodeName = "Has Target";

	// 기본 Blackboard Key 설정
	TargetPlayerKey.SelectedKeyName = "TargetPlayer";
}

bool UBTD_FCHasTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}

	// TargetPlayer가 유효한 Object인지 체크
	UObject* TargetObject = BlackboardComp->GetValueAsObject(TargetPlayerKey.SelectedKeyName);
	return TargetObject != nullptr;
}