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
			UE_LOG(LogTemp, Warning, TEXT("[BTT_FCClearLure] Set %s = false"), *HasLureTargetKey.SelectedKeyName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[BTT_FCClearLure] HasLureTargetKey is NOT set!"));
		}

		if (LureLocationKey.IsSet())
		{
			BlackboardComp->SetValueAsVector(LureLocationKey.SelectedKeyName, FVector::ZeroVector);
			UE_LOG(LogTemp, Warning, TEXT("[BTT_FCClearLure] Set %s = Zero"), *LureLocationKey.SelectedKeyName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[BTT_FCClearLure] LureLocationKey is NOT set!"));
		}

		// 디버그: 초기화 후 실제 값 확인
		const bool bAfterValue = BlackboardComp->GetValueAsBool(HasLureTargetKey.SelectedKeyName);
		UE_LOG(LogTemp, Warning, TEXT("[BTT_FCClearLure] After clear - bHasLureTarget = %s"),
			bAfterValue ? TEXT("true") : TEXT("false"));
	}

	UE_LOG(LogTemp, Warning, TEXT("[BTT_FCClearLure] Monster->bHasLureTarget = %s"),
		SoundHunter->bHasLureTarget ? TEXT("true") : TEXT("false"));

	return EBTNodeResult::Succeeded;
}

FString UBTT_FCClearLure::GetStaticDescription() const
{
	return FString::Printf(TEXT("Lure 상태 초기화\nHasLure: %s\nLocation: %s"),
		*HasLureTargetKey.SelectedKeyName.ToString(),
		*LureLocationKey.SelectedKeyName.ToString());
}