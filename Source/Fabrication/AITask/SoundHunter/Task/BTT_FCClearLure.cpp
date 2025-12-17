// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/SoundHunter/Task/BTT_FCClearLure.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/FCSoundHunter.h"

UBTT_FCClearLure::UBTT_FCClearLure()
{
	NodeName = TEXT("Clear Lure Target");

	// 블랙보드 키 필터 설정
	HasLureTargetKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCClearLure, HasLureTargetKey));
	LureLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCClearLure, LureLocationKey));
}

EBTNodeResult::Type UBTT_FCClearLure::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AIController 획득
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	// SoundHunter 획득
	AFCSoundHunter* SoundHunter = Cast<AFCSoundHunter>(AIController->GetPawn());
	if (!SoundHunter) return EBTNodeResult::Failed;

	// Monster의 Lure 상태 초기화
	SoundHunter->ClearLureTarget();

	// Blackboard의 Lure 상태 초기화
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		if (HasLureTargetKey.IsSet())
		{
			BlackboardComp->SetValueAsBool(HasLureTargetKey.SelectedKeyName, false);
		}
		if (LureLocationKey.IsSet())
		{
			BlackboardComp->SetValueAsVector(LureLocationKey.SelectedKeyName, FVector::ZeroVector);
		}
	}

	return EBTNodeResult::Succeeded;
}

FString UBTT_FCClearLure::GetStaticDescription() const
{
	return FString::Printf(TEXT("Lure 상태 초기화\nHasLure: %s\nLocation: %s"),
		*HasLureTargetKey.SelectedKeyName.ToString(),
		*LureLocationKey.SelectedKeyName.ToString());
}