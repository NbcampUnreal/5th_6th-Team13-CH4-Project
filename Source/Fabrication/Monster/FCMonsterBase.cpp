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
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// [중요] 기본 AIController 대신 우리가 만든 클래스 사용
	AIControllerClass = AFCMonsterAIController::StaticClass();

	// 스폰되거나 맵에 배치되면 자동으로 AI가 빙의됨
	// [멀티플레이] 서버에서만 AI가 빙의됨
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// [멀티플레이] 네트워크 업데이트 빈도 설정 (AI는 자주 움직이므로 높게 설정)
	SetNetUpdateFrequency(10.0f); // 초당 10번 업데이트
	SetMinNetUpdateFrequency(5.0f); // 최소 초당 5번

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

void AFCMonsterBase::ApplyStun(float Duration)
{
	if (!HasAuthority()) return;

	bIsStunned = true;

	// BehaviorTree에 즉시 반영하기 위해 이동 정지
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
	}

	// [버그 수정] 이미 스턴 중일 때 다시 ApplyStun 호출되면 기존 타이머 Clear
	if (GetWorldTimerManager().IsTimerActive(StunTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(StunTimerHandle);
	}
	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &AFCMonsterBase::EndStun, Duration, false);
}

void AFCMonsterBase::EndStun()
{
	bIsStunned = false;
}

void AFCMonsterBase::Multicast_PlayAttackAnim_Implementation()
{
	// [멀티플레이] 모든 클라이언트에서 공격 애니메이션 재생
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

void AFCMonsterBase::Multicast_PlayInvestigateAnim_Implementation()
{
	// [멀티플레이] 모든 클라이언트에서 수색 애니메이션 재생
	if (InvestigateMontage)
	{
		PlayAnimMontage(InvestigateMontage);
	}
}

