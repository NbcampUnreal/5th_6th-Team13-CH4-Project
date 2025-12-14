// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BTD_IsBeingWatched.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsBeingWatched::UBTD_IsBeingWatched()
{
	NodeName = "Is NOT Being Watched";
}

bool UBTD_IsBeingWatched::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}

	// "IsBeingWatched"가 false여야 AI가 진행 (플레이어가 안 보고 있어야 움직임)
	bool bIsWatched = BlackboardComp->GetValueAsBool(TEXT("IsBeingWatched"));
	return !bIsWatched;
}