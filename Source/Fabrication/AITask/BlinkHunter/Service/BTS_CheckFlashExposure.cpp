// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BlinkHunter/Service/BTS_CheckFlashExposure.h"
#include "Monster/FCMonsterBlinkHunter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CheckFlashExposure::UBTS_CheckFlashExposure()
{
	NodeName = "Check Flash Exposure";
	Interval = 0.1f; // 0.1초마다 체크
	RandomDeviation = 0.0f;

	// 키 필터 설정
	IsStunnedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_CheckFlashExposure, IsStunnedKey));
	FlashStunReadyKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_CheckFlashExposure, FlashStunReadyKey));
	TargetPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_CheckFlashExposure, TargetPlayerKey), AActor::StaticClass());
}

void UBTS_CheckFlashExposure::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		IsStunnedKey.ResolveSelectedKey(*BBAsset);
		FlashStunReadyKey.ResolveSelectedKey(*BBAsset);
		TargetPlayerKey.ResolveSelectedKey(*BBAsset);

		// [유효성 검증] 필수 Blackboard 키가 설정되었는지 확인
		if (!IsStunnedKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_CheckFlashExposure] IsStunnedKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
		if (!FlashStunReadyKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_CheckFlashExposure] FlashStunReadyKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
		if (!TargetPlayerKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_CheckFlashExposure] TargetPlayerKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
	}
}

void UBTS_CheckFlashExposure::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 노드 메모리 획득
	FBTSCheckFlashExposureMemory* Memory = reinterpret_cast<FBTSCheckFlashExposureMemory*>(NodeMemory);

	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return;
	}

	// BlinkHunter 획득
	AFCMonsterBlinkHunter* Monster = Cast<AFCMonsterBlinkHunter>(AICon->GetMonster());
	if (!Monster)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	// 1. IsStunned 블랙보드 동기화 (항상 실행)
	bool bCurrentlyStunned = Monster->bIsStunned;
	BlackboardComp->SetValueAsBool(IsStunnedKey.SelectedKeyName, bCurrentlyStunned);

	// 2. 스턴 종료 감지 (true → false) → TargetPlayer 초기화
	if (Memory->bWasPreviouslyStunned && !bCurrentlyStunned)
	{
		BlackboardComp->SetValueAsObject(TargetPlayerKey.SelectedKeyName, nullptr);
		Monster->TargetPlayer = nullptr;
	}

	// 3. 이전 스턴 상태 저장
	Memory->bWasPreviouslyStunned = bCurrentlyStunned;

	// 4. 스턴 상태면 Flash 노출 체크 안 함
	if (bCurrentlyStunned)
	{
		// 스턴 중에는 FlashStunReady도 false
		BlackboardComp->SetValueAsBool(FlashStunReadyKey.SelectedKeyName, false);
		return;
	}

	// 5. Flash 노출 여부 체크
	bool bExposed = Monster->IsExposedToFlash();

	// 6. 노출 시간 업데이트
	Monster->UpdateFlashExposureTime(DeltaSeconds, bExposed);

	// 7. FlashStunReady 블랙보드 갱신
	bool bStunReady = Monster->ShouldApplyFlashStun();
	BlackboardComp->SetValueAsBool(FlashStunReadyKey.SelectedKeyName, bStunReady);
}