// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/BaseHazardEvent.h"

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
