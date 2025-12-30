// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/SoundHunter/Task/BTT_FCPlayLureInvestigateMontage.h"
#include "Monster/FCSoundHunter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTT_FCPlayLureInvestigateMontage::UBTT_FCPlayLureInvestigateMontage()
{
	NodeName = TEXT("FC Play Lure Investigate Montage");

	// Abort Self로 설정해야 AbortTask가 호출됨
	bNotifyTaskFinished = true;
}

uint16 UBTT_FCPlayLureInvestigateMontage::GetInstanceMemorySize() const
{
	return sizeof(FBTTask_LureInvestigateMemory);
}

EBTNodeResult::Type UBTT_FCPlayLureInvestigateMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 시작 시간 저장
	FBTTask_LureInvestigateMemory* Memory = reinterpret_cast<FBTTask_LureInvestigateMemory*>(NodeMemory);
	if (Memory)
	{
		Memory->StartTime = GetWorld()->GetTimeSeconds();
	}

	// 부모 클래스의 ExecuteTask 호출 (몽타주 재생 + InProgress 반환)
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

EBTNodeResult::Type UBTT_FCPlayLureInvestigateMontage::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// SoundHunter에서 MinLureDuration 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return EBTNodeResult::Aborted;
	}

	AFCSoundHunter* SoundHunter = Cast<AFCSoundHunter>(AICon->GetPawn());
	if (!SoundHunter)
	{
		return EBTNodeResult::Aborted;
	}

	const float ElapsedTime = GetElapsedTime(OwnerComp, NodeMemory);
	const float MinDuration = SoundHunter->MinLureDuration;

	// MinLureDuration 안 지났으면 Abort 거부 (InProgress 유지)
	if (ElapsedTime < MinDuration)
	{
		UE_LOG(LogTemp, Log, TEXT("[BTT_FCPlayLureInvestigateMontage] Abort rejected (%.1f/%.1fs)"), ElapsedTime, MinDuration);
		return EBTNodeResult::InProgress;
	}

	// MinLureDuration 지났으면 정상 Abort
	UE_LOG(LogTemp, Log, TEXT("[BTT_FCPlayLureInvestigateMontage] Abort accepted (%.1fs)"), ElapsedTime);

	// 몽타주 정지
	StopMontage(SoundHunter);

	return EBTNodeResult::Aborted;
}

float UBTT_FCPlayLureInvestigateMontage::GetElapsedTime(const UBehaviorTreeComponent& OwnerComp, const uint8* NodeMemory) const
{
	const FBTTask_LureInvestigateMemory* Memory = reinterpret_cast<const FBTTask_LureInvestigateMemory*>(NodeMemory);
	if (!Memory)
	{
		return 0.0f;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	return CurrentTime - Memory->StartTime;
}

UAnimMontage* UBTT_FCPlayLureInvestigateMontage::GetMontage(AFCMonsterBase* Monster) const
{
	return Monster ? Monster->InvestigateMontage : nullptr;
}

void UBTT_FCPlayLureInvestigateMontage::PlayMontage(AFCMonsterBase* Monster)
{
	if (Monster)
	{
		Monster->Multicast_PlayInvestigateAnim();
		UE_LOG(LogTemp, Log, TEXT("[BTT_FCPlayLureInvestigateMontage] Playing Investigate Montage"));
	}
}

void UBTT_FCPlayLureInvestigateMontage::StopMontage(AFCMonsterBase* Monster)
{
	if (Monster)
	{
		Monster->Multicast_StopInvestigateAnim();
		UE_LOG(LogTemp, Log, TEXT("[BTT_FCPlayLureInvestigateMontage] Stopped Investigate Montage"));
	}
}

FString UBTT_FCPlayLureInvestigateMontage::GetStaticDescription() const
{
	return TEXT("Lure 도착 후 수색 애니메이션 재생\nMinLureDuration 동안 Abort 불가");
}
