// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BlinkHunter/Task/BTT_FCApplyFlashStun.h"
#include "Monster/FCMonsterBlinkHunter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FCApplyFlashStun::UBTT_FCApplyFlashStun()
{
	NodeName = "FC Apply Flash Stun";

	IsStunnedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCApplyFlashStun, IsStunnedKey));
	FlashStunReadyKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCApplyFlashStun, FlashStunReadyKey));
}

void UBTT_FCApplyFlashStun::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		IsStunnedKey.ResolveSelectedKey(*BBAsset);
		FlashStunReadyKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTT_FCApplyFlashStun::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	// BlinkHunter 획득
	AFCMonsterBlinkHunter* Monster = Cast<AFCMonsterBlinkHunter>(AICon->GetMonster());
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	// 이미 스턴 상태면 중복 적용하지 않음
	if (Monster->bIsStunned)
	{
		return EBTNodeResult::Succeeded;
	}

	// Flash 스턴 적용 (스턴 + 노출 시간 리셋)
	Monster->ApplyFlashStun();

	// 블랙보드 업데이트
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(IsStunnedKey.SelectedKeyName, true);
		BlackboardComp->SetValueAsBool(FlashStunReadyKey.SelectedKeyName, false);
	}

	return EBTNodeResult::Succeeded;
}
