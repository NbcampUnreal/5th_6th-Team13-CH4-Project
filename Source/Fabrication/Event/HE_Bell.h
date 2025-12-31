// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Bell.generated.h"

class USoundBase;
class UStaticMeshComponent;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Bell : public ABaseHazardEvent
{
	GENERATED_BODY()
public:
	AHE_Bell();

protected:
	virtual void BeginPlay() override;

	// 서버: 랜덤 시간 후 벨 울림
	void TriggerBell();

	// Overlap 안 플레이어 이동 체크
	void CheckPlayersInArea();

	// 감시 종료
	void EndBellEvent();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayBellSound();

	void CheckPlayerVelocityAfterDelay();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Scene;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> BellBoundary;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* BellSound;

	const FC_HazardDataRow* Row;
private:
	FTimerHandle RandomBellTimer;
	FTimerHandle MovementCheckTimer;
	FTimerHandle BellDurationTimer;

	bool bBellActive = false;
};
