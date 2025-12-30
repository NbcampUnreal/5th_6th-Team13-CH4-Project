// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BlinkHunter/Task/BTT_FCFindClosestPlayer.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "PlayerState/FCPlayerState.h"
#include "MonsterController/FCMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTT_FCFindClosestPlayer::UBTT_FCFindClosestPlayer()
{
	NodeName = "FC Find Closest Player";

	TargetPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FCFindClosestPlayer, TargetPlayerKey), AActor::StaticClass());
}

void UBTT_FCFindClosestPlayer::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetPlayerKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTT_FCFindClosestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	// Monster 획득
	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	// 모든 플레이어 가져오기
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(Monster->GetWorld(), AFCPlayerCharacter::StaticClass(), PlayerActors);

	if (PlayerActors.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	// 가장 가까운 플레이어 찾기
	AFCPlayerCharacter* ClosestPlayer = nullptr;
	float ClosestDistance = MAX_FLT;

	for (AActor* Actor : PlayerActors)
	{
		AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
		if (!Player) continue;

		// 죽은 플레이어(시체)는 타겟에서 제외
		if (AFCPlayerState* PS = Player->GetPlayerState<AFCPlayerState>())
		{
			if (PS->bIsDead)
			{
				continue;
			}
		}

		float Distance = FVector::Dist(Monster->GetActorLocation(), Player->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestPlayer = Player;
		}
	}

	if (!ClosestPlayer)
	{
		return EBTNodeResult::Failed;
	}

	// Monster의 Replicated 변수 업데이트
	Monster->TargetPlayer = ClosestPlayer;

	// Blackboard 업데이트
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetPlayerKey.SelectedKeyName, ClosestPlayer);
	}

	return EBTNodeResult::Succeeded;
}
