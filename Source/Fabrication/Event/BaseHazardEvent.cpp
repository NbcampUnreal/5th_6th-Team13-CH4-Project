// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/BaseHazardEvent.h"
#include "Event/LevelEventManager.h"


void ABaseHazardEvent::ApplyEffect()
{

}

void ABaseHazardEvent::StopEffect()
{

}

void ABaseHazardEvent::SetHazardType(EHazardType Type)
{
	HazardType = Type;
}

EHazardType ABaseHazardEvent::GetHazardType()
{
	return HazardType;
}

const FC_HazardDataRow* ABaseHazardEvent::GetMyHazardRow() const
{
	if (UWorld* World = GetWorld())
	{
		if (ULevelEventManager* Manager =
			World->GetSubsystem<ULevelEventManager>())
		{
			return Manager->GetHazardRow(HazardType);
		}
	}
	return nullptr;
}

