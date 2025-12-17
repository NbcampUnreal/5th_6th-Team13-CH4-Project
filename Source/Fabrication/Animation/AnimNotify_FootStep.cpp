// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_FootStep.h"
#include "Player/FCPlayerCharacter.h"

void UAnimNotify_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AFCPlayerCharacter* OwnerCharacter = Cast<AFCPlayerCharacter>(MeshComp->GetOwner()))
	{
		OwnerCharacter->FootStepAction();

	}
}
