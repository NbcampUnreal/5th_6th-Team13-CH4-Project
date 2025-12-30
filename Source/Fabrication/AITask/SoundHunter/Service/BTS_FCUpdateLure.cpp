// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/SoundHunter/Service/BTS_FCUpdateLure.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Monster/FCSoundHunter.h"

UBTS_FCUpdateLure::UBTS_FCUpdateLure()
{
	NodeName = TEXT("Update Lure Status");

	// 서비스 Tick 간격 설정
	Interval = 0.2f;
	RandomDeviation = 0.05f;

	// 블랙보드 키 필터 설정
	HasLureTargetKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_FCUpdateLure, HasLureTargetKey));
	LureLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_FCUpdateLure, LureLocationKey));
}

uint16 UBTS_FCUpdateLure::GetInstanceMemorySize() const
{
	return sizeof(FBTService_UpdateLureMemory);
}

void UBTS_FCUpdateLure::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	// 블랙보드 키 초기화
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		HasLureTargetKey.ResolveSelectedKey(*BBAsset);
		LureLocationKey.ResolveSelectedKey(*BBAsset);

		// [유효성 검증] 필수 Blackboard 키가 설정되었는지 확인
		if (!HasLureTargetKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_FCUpdateLure] HasLureTargetKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
		if (!LureLocationKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_FCUpdateLure] LureLocationKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
	}
}

void UBTS_FCUpdateLure::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// NodeMemory 획득
	FBTService_UpdateLureMemory* Memory = reinterpret_cast<FBTService_UpdateLureMemory*>(NodeMemory);
	if (!Memory) return;

	// AIController 획득
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	// SoundHunter 획득
	AFCSoundHunter* SoundHunter = Cast<AFCSoundHunter>(AIController->GetPawn());
	if (!SoundHunter) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	// [Vanish 상태 체크] Vanish 상태면 Lure 강제 해제 후 스킵
	if (SoundHunter->bIsVanished)
	{
		// [버그 수정] Vanish 중 Lure가 활성화되어 있으면 강제 해제 (이전 값 유지 방지)
		if (SoundHunter->bHasLureTarget)
		{
			SoundHunter->ClearLureTarget();
			if (HasLureTargetKey.IsSet())
			{
				BlackboardComp->SetValueAsBool(HasLureTargetKey.SelectedKeyName, false);
			}
		}
		return;
	}

	const bool bIsLureActive = SoundHunter->bHasLureTarget;
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	// Lure가 새로 활성화됨
	if (bIsLureActive && !Memory->bWasLureActive)
	{
		Memory->LureStartTime = CurrentTime;
		UE_LOG(LogTemp, Log, TEXT("[BTS_FCUpdateLure] Lure started"));
	}

	// Lure가 활성화 중일 때 - 타임아웃만 체크 (애니메이션은 BT 태스크에서 처리)
	if (bIsLureActive)
	{
		const float ElapsedTime = CurrentTime - Memory->LureStartTime;

		// 타임아웃 체크 (최대 지속 시간 초과 시 강제 클리어)
		if (ElapsedTime >= SoundHunter->LureDuration)
		{
			SoundHunter->ClearLureTarget();

			if (HasLureTargetKey.IsSet())
			{
				BlackboardComp->SetValueAsBool(HasLureTargetKey.SelectedKeyName, false);
			}
			if (LureLocationKey.IsSet())
			{
				BlackboardComp->SetValueAsVector(LureLocationKey.SelectedKeyName, FVector::ZeroVector);
			}

			UE_LOG(LogTemp, Log, TEXT("[BTS_FCUpdateLure] Lure timeout (%.1fs) - Cleared"), ElapsedTime);
		}
	}

	// 이전 상태 저장
	Memory->bWasLureActive = SoundHunter->bHasLureTarget;
}

FString UBTS_FCUpdateLure::GetStaticDescription() const
{
	return FString::Printf(TEXT("Lure 상태 업데이트\nHasLure: %s\nLocation: %s\n도착 또는 타임아웃 시 자동 해제"),
		*HasLureTargetKey.SelectedKeyName.ToString(),
		*LureLocationKey.SelectedKeyName.ToString());
}