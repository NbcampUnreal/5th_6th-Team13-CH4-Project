// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_FootStep.h"
#include "Player/FCPlayerCharacter.h"
#include "Monster/FCMonsterBase.h"

void UAnimNotify_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();

	// 플레이어 발소리
	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Owner))
	{
		Player->FootStepAction();
	}
	// 몬스터 발소리
	else if (AFCMonsterBase* Monster = Cast<AFCMonsterBase>(Owner))
	{
		Monster->FootStepAction();
	}
}
