// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCRespawn.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "MonsterController/FCMonsterAIController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

UBTT_FCRespawn::UBTT_FCRespawn()
{
	NodeName = "FC Respawn";
}

EBTNodeResult::Type UBTT_FCRespawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// 안전한 Respawn 위치 찾기
	FVector NewLocation;
	if (!GetRandomSpawnLocation(Monster, NewLocation))
	{
		// [버그 수정] 위치 찾기 실패 시 폴백: 현재 위치에서 그대로 재등장
		// 몬스터가 Vanish 상태로 영원히 남는 것 방지
		NewLocation = Monster->GetActorLocation();
	}

	// 1. 텔레포트
	Monster->SetActorLocation(NewLocation);

	// 2. 모습 보이기 & 충돌 켜기
	Monster->SetActorHiddenInGame(false);
	Monster->SetActorEnableCollision(true);

	// 3. 상태 복구
	Monster->bCanAttack = true;
	Monster->bIsStunned = false;

	return EBTNodeResult::Succeeded;
}

bool UBTT_FCRespawn::GetRandomSpawnLocation(AFCMonsterBase* Monster, FVector& OutLocation)
{
	if (!Monster) return false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Monster->GetWorld());
	if (!NavSystem) return false;

	// 맵 중앙 기준 (0,0,0)
	FVector Origin = FVector::ZeroVector;

	// 모든 플레이어 위치 가져오기
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(Monster->GetWorld(), AFCPlayerCharacter::StaticClass(), PlayerActors);

	// 최대 10번 시도
	for (int32 i = 0; i < 10; ++i)
	{
		FNavLocation RandomPoint;
		if (NavSystem->GetRandomPointInNavigableRadius(Origin, Monster->SpawnSearchRadius, RandomPoint))
		{
			bool bIsFarEnough = true;

			// 모든 플레이어와 거리 체크
			for (AActor* PlayerActor : PlayerActors)
			{
				float Distance = FVector::Dist(RandomPoint.Location, PlayerActor->GetActorLocation());
				if (Distance < Monster->MinSpawnDistanceFromPlayer)
				{
					bIsFarEnough = false; // 너무 가까움
					break;
				}
			}

			// 모든 플레이어와 충분히 멀다면 성공
			if (bIsFarEnough)
			{
				OutLocation = RandomPoint.Location;
				return true;
			}
		}
	}

	return false; // 적절한 위치 못 찾음
}