// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;

UCLASS()
class FABRICATION_API AFCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	
#pragma region AFCPlayerController Override
	AFCPlayerController();
	virtual void BeginPlay() override;
#pragma endregion

#pragma region PlayerInput
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> FCInputMappingContext;
#pragma endregion

};
