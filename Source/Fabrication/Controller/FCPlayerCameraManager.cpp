// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/FCPlayerCameraManager.h"

AFCPlayerCameraManager::AFCPlayerCameraManager()
{
	// Controller의 Pitch값을 제한
	ViewPitchMin = -70.f;
	ViewPitchMax = 70.f;
}