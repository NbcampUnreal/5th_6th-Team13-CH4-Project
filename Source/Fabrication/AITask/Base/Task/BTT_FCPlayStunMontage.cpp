// Fill out your copyright notice in the Description page of Project Settings.

#include "AITask/Base/Task/BTT_FCPlayStunMontage.h"
#include "Monster/FCMonsterBase.h"

UBTT_FCPlayStunMontage::UBTT_FCPlayStunMontage()
{
	NodeName = "FC Play Stun Montage";
}

UAnimMontage* UBTT_FCPlayStunMontage::GetMontage(AFCMonsterBase* Monster) const
{
	return Monster ? Monster->StunMontage : nullptr;
}

void UBTT_FCPlayStunMontage::PlayMontage(AFCMonsterBase* Monster)
{
	if (Monster)
	{
		Monster->Multicast_PlayStunAnim();
		// 로그는 Monster에서 처리 (FC_LOG_NET은 AActor에서만 사용 가능)
	}
}

void UBTT_FCPlayStunMontage::StopMontage(AFCMonsterBase* Monster)
{
	if (Monster)
	{
		Monster->Multicast_StopStunAnim();
	}
}
