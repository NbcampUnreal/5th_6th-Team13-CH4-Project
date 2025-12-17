// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_PlayerDie.h"
#include "Player/FCPlayerCharacter.h"

void UAnimNotify_PlayerDie::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AFCPlayerCharacter* FCPlayerCharacter = Cast<AFCPlayerCharacter>(MeshComp->GetOwner()))
	{
		FCPlayerCharacter->OnPlayerDiedProcessing();
	}
}
