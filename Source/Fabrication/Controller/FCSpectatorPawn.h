// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "FCSpectatorPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class FABRICATION_API AFCSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

#pragma region AFCSpectatorPawn Override
public:
	AFCSpectatorPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

#pragma endregion

#pragma region Function
public:
	void SetSpectateTarget(APawn* InTarget);
#pragma endregion

#pragma region Var
	UPROPERTY(ReplicatedUsing = OnRep_SpectateTarget)
	APawn* SpectateTarget;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Spectate")
	FVector FollowOffset = FVector(0.f, 0.f, 80.f);

	UPROPERTY(EditDefaultsOnly, Category = "Spectate")
	float LocationInterpSpeed = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spectate")
	float RotationInterpSpeed = 10.f;
#pragma endregion

#pragma region Func
public:
	UFUNCTION()
	void OnRep_SpectateTarget();
#pragma endregion



	
};
