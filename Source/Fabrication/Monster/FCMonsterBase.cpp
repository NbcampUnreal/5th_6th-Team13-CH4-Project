// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/FCMonsterBase.h"
//#include "Player/SurvivorCharacter.h" // 인클루드 경로 수정 반영
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/StateTreeComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "MonsterController/FCMonsterAIController.h" // 헤더 추가

// Sets default values

AFCMonsterBase::AFCMonsterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// StateTree 컴포넌트 생성
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	// [중요] 기본 AIController 대신 우리가 만든 클래스 사용
	AIControllerClass = AFCMonsterAIController::StaticClass();

	// 스폰되거나 맵에 배치되면 자동으로 AI가 빙의됨
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// 기본 이동 설정
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed_Normal;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// 1. Perception 컴포넌트 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// 2. 시야(Sight) 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f;           // 시야 거리 (15미터)
	SightConfig->LoseSightRadius = 2000.0f;       // 놓치는 거리 (좀 더 여유 둠)
	SightConfig->PeripheralVisionAngleDegrees = 90.0f; // 시야각 (좌우 90도 = 총 180도)
    
	// 나이, 소속 등 감지 설정
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// 컴포넌트에 설정 적용
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	
	bCanAttack = true;
	bIsStunned = false;

}

// Called when the game starts or when spawned
void AFCMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() == true)
	{
		if (StateTreeComponent)
		{
			StateTreeComponent->StartLogic();
		}
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AFCMonsterBase::OnTargetPerceptionUpdated);
		}
	}
	
}

void AFCMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// StateTree가 서버에서 상태를 바꾸면 클라도 알아야 함 (특히 타겟이나 스턴)
	//DOREPLIFETIME(AFCMonsterBase, TargetPlayer);
	DOREPLIFETIME(AFCMonsterBase, bIsStunned);
}

void AFCMonsterBase::SetMovementSpeed(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

bool AFCMonsterBase::TryAttackTarget()
{
	/*// 공격 가능 상태인지 체크
	if (!bCanAttack || !TargetPlayer || bIsStunned) return false;

	float Dist = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
	if (Dist <= AttackRange)
	{
		Server_Attack(TargetPlayer);
		return true;
	}
	*/
	return false;
}

bool AFCMonsterBase::GetInvestigateLocation(FVector& OutLocation)
{
	// 타겟이 아예 없으면 그냥 랜덤 순찰
	/*
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
	*/
	return false;
}

void AFCMonsterBase::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	/*
	ASurvivorCharacter* Survivor = Cast<ASurvivorCharacter>(Actor);
	if (!Survivor) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		// [발견] 시야에 들어옴 -> 추격 대상 설정
		SeenPlayer = Survivor;
		TargetPlayer = Survivor; // StateTree가 이걸 보고 Chase 상태로 전이
		LastStimulusLocation = Stimulus.StimulusLocation; // 위치 기억
	}
	else
	{
		// [놓침] 시야에서 사라짐 (벽 뒤로 숨음)
		// 바로 포기하지 않고, 마지막 위치까지는 가보게 함 (수색 모드)
		if (SeenPlayer == Survivor)
		{
			SeenPlayer = nullptr;
			// TargetPlayer는 바로 null로 안 만듦. (StateTree에서 '마지막 위치 수색' 로직 후 초기화)
		}
	}
	*/
}
/*
ASurvivorCharacter* AFCMonsterBase::GetSeenPlayer()
{
	return SeenPlayer; // 벽 뒤에 있으면 nullptr이 리턴됨 (안전!)
}
*/

/*
ASurvivorCharacter* AFCMonsterBase::FindNearestSurvivor(float SearchRadius)
{
	ASurvivorCharacter* Nearest = nullptr;
	float MinDist = SearchRadius;

	// 월드의 모든 생존자 검색
	// (최적화를 위해 나중에는 GameState의 Player 배열을 참조하는 것이 좋음)
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivorCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ASurvivorCharacter* Survivor = Cast<ASurvivorCharacter>(Actor);
		if (Survivor) // && Survivor->IsAlive() 체크 필요 (B팀 구현 후)
		{
			float Dist = FVector::Dist(GetActorLocation(), Survivor->GetActorLocation());
			if (Dist < MinDist)
			{
				MinDist = Dist;
				Nearest = Survivor;
			}
		}
	}
	
	return Nearest;
}
*/

void AFCMonsterBase::ApplyStun(float Duration)
{
	if (!HasAuthority()) return;

	bIsStunned = true;
    
	// StateTree에 즉시 반영하기 위해 이동 정지
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
	}

	// StateTreeComponent->SendStateTreeEvent()를 통해 트리 로직을 깨울 수도 있음
    
	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &AFCMonsterBase::EndStun, Duration, false);
}

void AFCMonsterBase::ResetAttackCooldown()
{
	bCanAttack = true;
}

void AFCMonsterBase::EndStun()
{
	bIsStunned = false;
}

void AFCMonsterBase::Vanish()
{
	// 이미 사라진 상태면 패스
	if (IsHidden()) return;

	// 1. 모습 숨기기 & 충돌 끄기
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// 2. AI 로직 정지 (StateTree 멈춤)
	if (StateTreeComponent)
	{
		StateTreeComponent->StopLogic(TEXT("Vanish"));
	}
    
	// 이동 정지
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
	}

	// 타겟 초기화
	//TargetPlayer = nullptr;

	// 3. 재소환 타이머 시작
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AFCMonsterBase::Respawn, RespawnCooldown, false);
    
	// 로그 (디버깅용)
	UE_LOG(LogTemp, Warning, TEXT("Monster Vanished. Respawning in %f sec"), RespawnCooldown);
}

void AFCMonsterBase::Respawn()
{
	// 1. 적절한 위치 찾기
	FVector NewLocation;
	if (GetRandomSpawnLocation(NewLocation))
	{
		// 위치 이동 (텔레포트)
		SetActorLocation(NewLocation);

		// 2. 모습 보이기 & 충돌 켜기
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);

		// 3. AI 로직 재가동
		if (StateTreeComponent)
		{
			StateTreeComponent->StartLogic();
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
	/*
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return false;

	FVector Origin = FVector::ZeroVector; // 맵 중앙 혹은 특정 기준점
	// 만약 맵 중앙을 기준으로 하고 싶다면 (0,0,0)을 쓰거나, GameState에서 맵 중심을 가져올 수도 있음.
    
	// 생존자들 위치 가져오기
	TArray<AActor*> Survivors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivorCharacter::StaticClass(), Survivors);

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
	*/
	return false; // 적절한 위치 못 찾음
}

void AFCMonsterBase::Multicast_PlayAttackAnim_Implementation()
{
	// PlayAnimMontage(...) 구현
}
/*
void AFCMonsterBase::Server_Attack_Implementation(ASurvivorCharacter* Target)
{
	
	if (!Target) return;

	// 1. 데미지 적용
	UGameplayStatics::ApplyDamage(Target, DamagePerAttack, GetController(), this, UDamageType::StaticClass());

	// 2. 공격 애니메이션 재생 (멀티캐스트)
	Multicast_PlayAttackAnim();

	// 3. 공격 후 사라짐 처리 (Vanish)
	// 바로 사라지면 어색하므로, 애니메이션 길이만큼 기다리거나 별도 함수로 호출
	// 여기서는 예시로 1.5초 후 사라지게 설정 (공격 모션 끝난 뒤)
	FTimerHandle VanishTimer;
	GetWorldTimerManager().SetTimer(VanishTimer, this, &AMonsterBase::Vanish, 1.5f, false);
	
}*/

