// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Service/BTS_FCCheckPlayerDistance.h"
#include "Monster/FCMonsterBase.h"
#include "MonsterController/FCMonsterAIController.h"
#include "Player/FCPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Kismet/GameplayStatics.h"

UBTS_FCCheckPlayerDistance::UBTS_FCCheckPlayerDistance()
{
	NodeName = "Check Player Distance";
	Interval = 0.1f;         // 0.1초마다 검사
	RandomDeviation = 0.0f;  // 정확한 주기로 실행

	// 블랙보드 키 필터 설정
	TargetPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_FCCheckPlayerDistance, TargetPlayerKey), AActor::StaticClass());
	OutputKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_FCCheckPlayerDistance, OutputKey));
}

void UBTS_FCCheckPlayerDistance::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	// 블랙보드 에셋에서 키 정보 해결
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetPlayerKey.ResolveSelectedKey(*BBAsset);
		OutputKey.ResolveSelectedKey(*BBAsset);

		// [유효성 검증] 필수 Blackboard 키가 설정되었는지 확인
		if (!TargetPlayerKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_FCCheckPlayerDistance] TargetPlayerKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
		if (!OutputKey.IsSet())
		{
			UE_LOG(LogTemp, Error, TEXT("[BTS_FCCheckPlayerDistance] OutputKey가 설정되지 않음! BT 에디터에서 확인 필요"));
		}
	}
}

FString UBTS_FCCheckPlayerDistance::GetStaticDescription() const
{
	FString RangeDesc = bUseMonsterAttackRange
		? TEXT("Monster AttackRange")
		: FString::Printf(TEXT("%.0f cm"), CustomCheckRange);

	FString TargetDesc = bCheckAnyPlayer
		? TEXT("Any Player")
		: TEXT("TargetPlayer Only");

	return FString::Printf(TEXT("Range: %s\nTarget: %s\nOutput: %s"),
		*RangeDesc, *TargetDesc, *OutputKey.SelectedKeyName.ToString());
}

void UBTS_FCCheckPlayerDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AFCMonsterAIController* AIController = Cast<AFCMonsterAIController>(OwnerComp.GetAIOwner());
	AFCMonsterBase* Monster = AIController ? AIController->GetMonster() : nullptr;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Monster || !Blackboard)
	{
		return;
	}

	// TargetPlayer 모드인데 Target이 없으면 서비스 스킵
	if (!bCheckAnyPlayer)
	{
		AFCPlayerCharacter* Target = Cast<AFCPlayerCharacter>(Blackboard->GetValueAsObject(TargetPlayerKey.SelectedKeyName));
		if (!Target)
		{
			return;
		}
	}

	// 체크할 거리 결정
	float CheckRange = bUseMonsterAttackRange ? Monster->AttackRange : CustomCheckRange;

	bool bPlayerInRange = false;

	if (bCheckAnyPlayer)
	{
		// 모든 플레이어 중 가장 가까운 플레이어 체크
		TArray<AActor*> PlayerActors;
		UGameplayStatics::GetAllActorsOfClass(Monster->GetWorld(), AFCPlayerCharacter::StaticClass(), PlayerActors);

		for (AActor* Actor : PlayerActors)
		{
			if (!Actor) continue;

			float Distance = Monster->GetDistanceTo(Actor);
			if (Distance <= CheckRange)
			{
				bPlayerInRange = true;
				break;
			}
		}
	}
	else
	{
		// TargetPlayer만 체크
		AFCPlayerCharacter* Target = Cast<AFCPlayerCharacter>(Blackboard->GetValueAsObject(TargetPlayerKey.SelectedKeyName));
		if (Target)
		{
			float Distance = Monster->GetDistanceTo(Target);
			bPlayerInRange = (Distance <= CheckRange);
		}
	}

	// 블랙보드 값 갱신
	Blackboard->SetValueAsBool(OutputKey.SelectedKeyName, bPlayerInRange);
}
