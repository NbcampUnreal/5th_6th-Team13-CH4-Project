// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Decorator/BTD_FCHasSeenPlayer.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_FCHasSeenPlayer::UBTD_FCHasSeenPlayer()
{
	NodeName = "Has Seen Player";
}

bool UBTD_FCHasSeenPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// Blackboard에서 SeenPlayer 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}

	// SeenPlayer가 유효한지 체크 (현재 시야에 보이는지)
	UObject* SeenPlayerObj = BlackboardComp->GetValueAsObject(TEXT("SeenPlayer"));
	return SeenPlayerObj != nullptr;
}
