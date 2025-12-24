// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCRespawn.h"
#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "PlayerState/FCPlayerState.h"
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
		// 폴백: 현재 위치를 NavMesh에 프로젝션하여 유효한 위치 확보
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Monster->GetWorld());
		bool bFoundFallback = false;

		if (NavSystem)
		{
			FNavLocation NavLocation;
			// 넓은 범위(500cm)에서 NavMesh 프로젝션 시도
			if (NavSystem->ProjectPointToNavigation(
				Monster->GetActorLocation(),
				NavLocation,
				FVector(500.0f, 500.0f, 500.0f)))
			{
				NewLocation = NavLocation.Location;
				bFoundFallback = true;
				UE_LOG(LogTemp, Warning, TEXT("BTT_FCRespawn: 랜덤 위치 실패, NavMesh 프로젝션 폴백 사용"));
			}
		}

		if (!bFoundFallback)
		{
			// 최후의 수단: 현재 위치 (NavMesh 없는 레벨이거나 심각한 문제)
			NewLocation = Monster->GetActorLocation();
			UE_LOG(LogTemp, Error, TEXT("BTT_FCRespawn: NavMesh 프로젝션 실패! 현재 위치에서 리스폰 - 이동 불가 상태일 수 있음"));
		}
	}

	// 1. 텔레포트
	Monster->SetActorLocation(NewLocation);

	// 2. 모습 보이기 & 충돌 켜기
	Monster->SetActorHiddenInGame(false);
	Monster->SetActorEnableCollision(true);

	// 3. 상태 복구
	Monster->bCanAttack = true;
	Monster->bIsStunned = false;
	Monster->bIsVanished = false;

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

	// NavMesh 프로젝션용 범위 (XY 100cm, Z 200cm)
	const FVector ProjectionExtent(100.0f, 100.0f, 200.0f);

	// 최대 20번 시도 (기존 10번 → 20번으로 증가)
	for (int32 i = 0; i < 20; ++i)
	{
		FNavLocation RandomPoint;
		if (NavSystem->GetRandomPointInNavigableRadius(Origin, Monster->SpawnSearchRadius, RandomPoint))
		{
			// 추가 검증: 해당 위치가 NavMesh 위에 유효한지 ProjectPointToNavigation으로 재확인
			FNavLocation ProjectedLocation;
			if (!NavSystem->ProjectPointToNavigation(RandomPoint.Location, ProjectedLocation, ProjectionExtent))
			{
				// NavMesh 프로젝션 실패 - 이 위치는 실제로 이동 불가능할 수 있음
				continue;
			}

			bool bIsFarEnough = true;

			// 살아있는 플레이어와만 거리 체크 (시체 제외)
			for (AActor* PlayerActor : PlayerActors)
			{
				AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(PlayerActor);
				if (!Player) continue;

				// 죽은 플레이어(시체)는 거리 체크에서 제외
				if (AFCPlayerState* PS = Player->GetPlayerState<AFCPlayerState>())
				{
					if (PS->bIsDead)
					{
						continue;
					}
				}

				float Distance = FVector::Dist(ProjectedLocation.Location, PlayerActor->GetActorLocation());
				if (Distance < Monster->MinSpawnDistanceFromPlayer)
				{
					bIsFarEnough = false; // 너무 가까움
					break;
				}
			}

			// 모든 플레이어와 충분히 멀다면 성공
			if (bIsFarEnough)
			{
				OutLocation = ProjectedLocation.Location;
				return true;
			}
		}
	}

	return false; // 적절한 위치 못 찾음
}