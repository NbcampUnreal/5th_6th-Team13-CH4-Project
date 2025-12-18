// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/SoundHunter/Task/BTT_FCClearHeardSound.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/FCSoundHunter.h"

UBTT_FCClearHeardSound::UBTT_FCClearHeardSound()
{
	NodeName = TEXT("Clear Heard Sound");

	// 블랙보드 키 필터 설정
	HasHeardSoundKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCClearHeardSound, HasHeardSoundKey));
	LastHeardLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCClearHeardSound, LastHeardLocationKey));
}

EBTNodeResult::Type UBTT_FCClearHeardSound::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AIController 획득
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	// SoundHunter 획득
	AFCSoundHunter* SoundHunter = Cast<AFCSoundHunter>(AIController->GetPawn());
	if (!SoundHunter) return EBTNodeResult::Failed;

	// Monster의 HeardSound 상태 초기화
	SoundHunter->ClearHeardSound();

	// Blackboard의 HeardSound 상태 초기화
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		if (HasHeardSoundKey.IsSet())
		{
			BlackboardComp->SetValueAsBool(HasHeardSoundKey.SelectedKeyName, false);
			UE_LOG(LogTemp, Warning, TEXT("[BTT_FCClearHeardSound] Set %s = false"), *HasHeardSoundKey.SelectedKeyName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[BTT_FCClearHeardSound] HasHeardSoundKey is NOT set!"));
		}

		if (LastHeardLocationKey.IsSet())
		{
			BlackboardComp->SetValueAsVector(LastHeardLocationKey.SelectedKeyName, FVector::ZeroVector);
			UE_LOG(LogTemp, Warning, TEXT("[BTT_FCClearHeardSound] Set %s = Zero"), *LastHeardLocationKey.SelectedKeyName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[BTT_FCClearHeardSound] LastHeardLocationKey is NOT set!"));
		}

		// 디버그: 초기화 후 실제 값 확인
		const bool bAfterValue = BlackboardComp->GetValueAsBool(HasHeardSoundKey.SelectedKeyName);
		UE_LOG(LogTemp, Warning, TEXT("[BTT_FCClearHeardSound] After clear - bHasHeardSound = %s"),
			bAfterValue ? TEXT("true") : TEXT("false"));
	}

	UE_LOG(LogTemp, Warning, TEXT("[BTT_FCClearHeardSound] Monster->bHasHeardSound = %s"),
		SoundHunter->bHasHeardSound ? TEXT("true") : TEXT("false"));

	return EBTNodeResult::Succeeded;
}

FString UBTT_FCClearHeardSound::GetStaticDescription() const
{
	return FString::Printf(TEXT("HeardSound 상태 초기화\nHasHeardSound: %s\nLocation: %s"),
		*HasHeardSoundKey.SelectedKeyName.ToString(),
		*LastHeardLocationKey.SelectedKeyName.ToString());
}
