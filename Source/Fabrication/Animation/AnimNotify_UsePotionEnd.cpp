// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_UsePotionEnd.h"

#include "Player/FCPlayerCharacter.h"

void UAnimNotify_UsePotionEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AFCPlayerCharacter* OwnerCharacter = Cast<AFCPlayerCharacter>(MeshComp->GetOwner()))
	{
		OwnerCharacter->CheckingSelectSlot();
	}
}
