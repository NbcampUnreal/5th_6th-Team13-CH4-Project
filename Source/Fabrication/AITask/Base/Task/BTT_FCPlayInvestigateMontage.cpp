// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCPlayInvestigateMontage.h"
#include "Monster/FCMonsterBase.h"

UBTT_FCPlayInvestigateMontage::UBTT_FCPlayInvestigateMontage()
{
	NodeName = "FC Play Investigate Montage";
}

UAnimMontage* UBTT_FCPlayInvestigateMontage::GetMontage(AFCMonsterBase* Monster) const
{
	return Monster ? Monster->InvestigateMontage : nullptr;
}

void UBTT_FCPlayInvestigateMontage::PlayMontage(AFCMonsterBase* Monster)
{
	if (Monster)
	{
		Monster->Multicast_PlayInvestigateAnim();
	}
}

void UBTT_FCPlayInvestigateMontage::StopMontage(AFCMonsterBase* Monster)
{
	if (Monster)
	{
		Monster->Multicast_StopInvestigateAnim();
	}
}
