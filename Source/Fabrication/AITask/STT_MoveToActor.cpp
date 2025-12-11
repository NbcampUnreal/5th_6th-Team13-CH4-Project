// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/STT_MoveToActor.h"
#include "AIController.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Monster/FCMonsterBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_MoveToActor::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 1. 인스턴스 데이터 가져오기
	const FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	// 2. AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(Context.GetOwner());
	if (!AICon) return EStateTreeRunStatus::Failed;

	// 3. Monster 획득
	AFCMonsterBase* Monster = AICon->GetMonster();
	if (!Monster) return EStateTreeRunStatus::Failed;

	// 4. 목표 Actor가 유효한지 확인
	if (!InstanceData.TargetActor || !IsValid(InstanceData.TargetActor))
	{
		return EStateTreeRunStatus::Failed;
	}

	// 5. 이동 시작 (FAIMoveRequest 사용)
	FAIMoveRequest MoveRequest(InstanceData.TargetActor);
	MoveRequest.SetAcceptanceRadius(InstanceData.AcceptanceRadius);
	MoveRequest.SetUsePathfinding(true);
	//MoveRequest.SetStopOnOverlap(InstanceData.bStopOnOverlap);

	const FPathFollowingRequestResult Result = AICon->MoveTo(MoveRequest);

	if (Result.Code == EPathFollowingRequestResult::Failed)
	{
		return EStateTreeRunStatus::Failed;
	}

	// 6. 이동 중 상태로 전환 (Tick에서 완료 확인)
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_MoveToActor::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// 1. 인스턴스 데이터 가져오기
	const FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	// 2. 목표 Actor가 사라졌는지 확인
	if (!InstanceData.TargetActor || !IsValid(InstanceData.TargetActor))
	{
		return EStateTreeRunStatus::Failed;
	}

	// 3. AIController 획득
	AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(Context.GetOwner());
	if (!AICon) return EStateTreeRunStatus::Failed;

	// 4. PathFollowingComponent로 이동 상태 확인
	const UPathFollowingComponent* PathFollowing = AICon->GetPathFollowingComponent();
	if (!PathFollowing) return EStateTreeRunStatus::Failed;

	const EPathFollowingStatus::Type Status = PathFollowing->GetStatus();

	switch (Status)
	{
	case EPathFollowingStatus::Moving:
		return EStateTreeRunStatus::Running; // 계속 이동 중

	case EPathFollowingStatus::Idle:
		return EStateTreeRunStatus::Succeeded; // 목표 도달

	case EPathFollowingStatus::Waiting:
		return EStateTreeRunStatus::Running; // 대기 중 (계속 진행)

	case EPathFollowingStatus::Paused:
		return EStateTreeRunStatus::Running; // 일시정지 (계속 진행)

	default:
		return EStateTreeRunStatus::Failed; // 실패
	}
}

void FSTT_MoveToActor::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// Task 종료 시 이동 정지
	if (AFCMonsterAIController* AICon = Cast<AFCMonsterAIController>(Context.GetOwner()))
	{
		AICon->StopMovement();
	}
}