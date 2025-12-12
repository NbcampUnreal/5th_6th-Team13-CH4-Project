// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/FCMonsterBase.h"
#include "Player/FCPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "MonsterController/FCMonsterAIController.h"

// Sets default values

AFCMonsterBase::AFCMonsterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// [중요] 기본 AIController 대신 우리가 만든 클래스 사용
	AIControllerClass = AFCMonsterAIController::StaticClass();

	// 스폰되거나 맵에 배치되면 자동으로 AI가 빙의됨
	// [멀티플레이] 서버에서만 AI가 빙의됨
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// [멀티플레이] 네트워크 업데이트 빈도 설정 (AI는 자주 움직이므로 높게 설정)
	NetUpdateFrequency = 10.0f; // 초당 10번 업데이트
	MinNetUpdateFrequency = 5.0f; // 최소 초당 5번

	// 기본 상태 초기화
	bCanAttack = true;
	bIsStunned = false;
}

void AFCMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	// [AI 이동 설정] CharacterMovementComponent 설정
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		// 기본 이동 속도
		MovementComp->MaxWalkSpeed = MoveSpeed_Normal;

		// 이동 방향으로 자동 회전
		MovementComp->bOrientRotationToMovement = true;

		// Yaw만 회전 (Pitch, Roll 제한)
		MovementComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 초당 540도

		// [멀티플레이] CharacterMovementComponent 복제 설정
		MovementComp->SetIsReplicated(true);
	}

	// [AI 회전 제한] Controller 회전 무시 (AI는 자동 회전)
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// [멀티플레이] Perception은 AIController에서 관리
	// AIController의 OnPossess에서 델리게이트 바인딩됨
}

void AFCMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// BehaviorTree가 서버에서 상태를 바꾸면 클라도 알아야 함 (특히 타겟이나 스턴)
	DOREPLIFETIME(AFCMonsterBase, TargetPlayer);
	DOREPLIFETIME(AFCMonsterBase, bIsStunned);
	DOREPLIFETIME(AFCMonsterBase, bCanAttack);
	DOREPLIFETIME(AFCMonsterBase, SeenPlayer);
	DOREPLIFETIME(AFCMonsterBase, LastStimulusLocation);
}

void AFCMonsterBase::SetMovementSpeed(float NewSpeed)
{
	// AI 로직은 서버에서만 실행되어야 함
	if (!HasAuthority()) return;

	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

bool AFCMonsterBase::TryAttackTarget()
{
	// AI 로직은 서버에서만 실행되어야 함
	if (!HasAuthority()) return false;

	// 공격 가능 상태인지 체크
	if (!bCanAttack || !TargetPlayer || bIsStunned) return false;

	float Dist = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
	if (Dist <= AttackRange)
	{
		// [멀티플레이] 이미 서버에서 실행 중이므로 RPC 대신 직접 호출
		// 1. 데미지 적용
		UGameplayStatics::ApplyDamage(TargetPlayer, DamagePerAttack, GetController(), this, UDamageType::StaticClass());

		// 2. 공격 애니메이션 재생 (멀티캐스트)
		Multicast_PlayAttackAnim();

		// 3. 공격 후 사라짐 처리 (Vanish)
		// 공격 모션 끝난 뒤 사라지게 설정 (Hit & Run 패턴)
		FTimerHandle VanishTimer;
		GetWorldTimerManager().SetTimer(VanishTimer, this, &AFCMonsterBase::Vanish, 1.5f, false);

		return true;
	}
	return false;
}

bool AFCMonsterBase::GetInvestigateLocation(FVector& OutLocation)
{
	// AI 로직은 서버에서만 실행되어야 함
	if (!HasAuthority()) return false;

	// 타겟이 아예 없으면 그냥 랜덤 순찰
	if (!TargetPlayer) return false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return false;

	// 전략: 몬스터와 플레이어 사이의 방향 벡터를 구함
	FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	// 그 방향으로 일정 거리(예: 5~10미터) 앞의 지점을 기준점으로 잡음
	FVector SearchOrigin = GetActorLocation() + (DirectionToPlayer * 800.0f);

	// 그 기준점 근처의 "네비게이션 가능한" 랜덤 위치를 찾음
	FNavLocation NavLoc;
	if (NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, 500.0f, NavLoc))
	{
		OutLocation = NavLoc.Location;
		return true;
	}
	return false;
}

AFCPlayerCharacter* AFCMonsterBase::GetSeenPlayer()
{
	// AI 로직은 서버에서만 실행되어야 함
	if (!HasAuthority()) return nullptr;

	return SeenPlayer; // 벽 뒤에 있으면 nullptr이 리턴됨 (안전!)
}

void AFCMonsterBase::ApplyStun(float Duration)
{
	if (!HasAuthority()) return;

	bIsStunned = true;

	// BehaviorTree에 즉시 반영하기 위해 이동 정지
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
	}

	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &AFCMonsterBase::EndStun, Duration, false);
}

void AFCMonsterBase::EndStun()
{
	bIsStunned = false;
}

void AFCMonsterBase::Vanish()
{
	// 서버에서만 실행
	if (!HasAuthority()) return;

	// 이미 사라진 상태면 패스
	if (IsHidden()) return;

	// 1. 모습 숨기기 & 충돌 끄기
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// 2. AI 로직 정지
	if (AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(GetController()))
	{
		AICon->StopMovement();
		AICon->StopBehaviorTree();
	}

	// 타겟 초기화
	TargetPlayer = nullptr;
	SeenPlayer = nullptr;

	// 3. 재소환 타이머 시작
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AFCMonsterBase::Respawn, RespawnCooldown, false);

	UE_LOG(LogTemp, Warning, TEXT("Monster Vanished. Respawning in %f sec"), RespawnCooldown);
}

void AFCMonsterBase::Respawn()
{
	// 서버에서만 실행
	if (!HasAuthority()) return;

	// 1. 적절한 위치 찾기
	FVector NewLocation;
	if (GetRandomSpawnLocation(NewLocation))
	{
		// 위치 이동 (텔레포트)
		SetActorLocation(NewLocation);

		// 2. 모습 보이기 & 충돌 켜기
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);

		// 3. AI 로직 재가동 (BehaviorTree 재시작)
		if (AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(GetController()))
		{
			AICon->RestartBehaviorTree();
		}

		// 4. 공격 상태 리셋
		bCanAttack = true;
		bIsStunned = false;

		UE_LOG(LogTemp, Warning, TEXT("Monster Respawned at %s"), *NewLocation.ToString());
	}
	else
	{
		// 위치 찾기 실패 시 1초 뒤 재시도
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AFCMonsterBase::Respawn, 1.0f, false);
		UE_LOG(LogTemp, Warning, TEXT("Failed to find spawn location. Retrying..."));
	}
}

bool AFCMonsterBase::GetRandomSpawnLocation(FVector& OutLocation)
{
	// 서버에서만 실행
	if (!HasAuthority()) return false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return false;

	FVector Origin = FVector::ZeroVector; // 맵 중앙 혹은 특정 기준점
	// 만약 맵 중앙을 기준으로 하고 싶다면 (0,0,0)을 쓰거나, GameState에서 맵 중심을 가져올 수도 있음.

	// 생존자들 위치 가져오기
	TArray<AActor*> Survivors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFCPlayerCharacter::StaticClass(), Survivors);

	// 최대 10번 시도해서 적절한 위치 찾기
	for (int32 i = 0; i < 10; ++i)
	{
		FNavLocation RandomPt;
		// 맵 전역(혹은 원점 기준) 반경 내 랜덤 위치
		// *참고: 만약 맵 전체에서 랜덤을 원하면 Origin을 0,0,0으로 하고 Radius를 매우 크게 잡으세요.
		if (NavSystem->GetRandomPointInNavigableRadius(Origin, SpawnSearchRadius, RandomPt))
		{
			bool bIsFarEnough = true;

			// 모든 생존자와 거리 체크
			for (AActor* Survivor : Survivors)
			{
				if (Survivor && FVector::Dist(RandomPt.Location, Survivor->GetActorLocation()) < MinSpawnDistanceFromPlayer)
				{
					bIsFarEnough = false; // 너무 가까움
					break;
				}
			}

			// 모든 생존자와 충분히 멀다면 성공
			if (bIsFarEnough)
			{
				OutLocation = RandomPt.Location;
				return true;
			}
		}
	}
	return false; // 적절한 위치 못 찾음
}

void AFCMonsterBase::Multicast_PlayAttackAnim_Implementation()
{
	// [멀티플레이] 모든 클라이언트에서 공격 애니메이션 재생
	// PlayAnimMontage(...) 구현
}

