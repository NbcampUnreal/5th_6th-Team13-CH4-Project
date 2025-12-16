// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/BlinkHunter/Service/BTS_CheckPlayerGaze.h"
#include "Monster/FCMonsterBlinkHunter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Player/FCPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTS_CheckPlayerGaze::UBTS_CheckPlayerGaze()
{
	NodeName = "Check Player Gaze";
	Interval = 0.1f; // 0.1초마다 체크
	RandomDeviation = 0.0f;

	// 키 필터 설정
	IsBeingWatchedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_CheckPlayerGaze, IsBeingWatchedKey));
	TargetPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_CheckPlayerGaze, TargetPlayerKey), AActor::StaticClass());
}

void UBTS_CheckPlayerGaze::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		IsBeingWatchedKey.ResolveSelectedKey(*BBAsset);
		TargetPlayerKey.ResolveSelectedKey(*BBAsset);
	}
}

void UBTS_CheckPlayerGaze::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 노드 메모리 획득
	FBTSCheckPlayerGazeMemory* Memory = reinterpret_cast<FBTSCheckPlayerGazeMemory*>(NodeMemory);

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

	// 스턴 상태면 체크 안 함 (Freeze 유지)
	if (Monster->bIsStunned)
	{
		Monster->SetFrozen(true);
		return;
	}

	// 모든 플레이어 순회하며 관찰자 찾기
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(Monster->GetWorld(), AFCPlayerCharacter::StaticClass(), PlayerActors);

	bool bCurrentlyWatched = false;
	AFCPlayerCharacter* CurrentWatcher = nullptr;
	float ClosestDistance = MAX_FLT;

	for (AActor* Actor : PlayerActors)
	{
		AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
		if (!Player) continue;

		// 이 플레이어가 몬스터를 보고 있는지 체크
		if (Monster->IsPlayerLookingAtMe(Player))
		{
			bCurrentlyWatched = true;

			// 가장 가까운 관찰자 추적 (마지막으로 본 플레이어 후보)
			float Distance = Monster->GetDistanceTo(Player);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				CurrentWatcher = Player;
			}
		}
	}

	// 1. 관찰 중이면 마지막 관찰자 업데이트
	if (bCurrentlyWatched && CurrentWatcher)
	{
		Memory->LastWatchingPlayer = CurrentWatcher;
	}

	// 2. 관찰이 끝났을 때 (true → false) TargetPlayer 설정
	if (Memory->bWasPreviouslyWatched && !bCurrentlyWatched)
	{
		// 마지막으로 본 플레이어를 타겟으로 설정
		if (Memory->LastWatchingPlayer.IsValid())
		{
			AFCPlayerCharacter* LastWatcher = Memory->LastWatchingPlayer.Get();
			BlackboardComp->SetValueAsObject(TargetPlayerKey.SelectedKeyName, LastWatcher);
			Monster->TargetPlayer = LastWatcher;
		}
	}

	// 3. 현재 상태 저장 (다음 틱에서 비교용)
	Memory->bWasPreviouslyWatched = bCurrentlyWatched;

	// 4. Blackboard 업데이트
	BlackboardComp->SetValueAsBool(IsBeingWatchedKey.SelectedKeyName, bCurrentlyWatched);

	// 5. Monster의 Replicated 변수 업데이트 (클라이언트 애니메이션용)
	Monster->bIsBeingWatched = bCurrentlyWatched;

	// 6. Freeze 상태 조절 (BT에서 설정한 속도 보존)
	Monster->SetFrozen(bCurrentlyWatched);
}