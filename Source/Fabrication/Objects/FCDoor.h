#pragma once

#include "CoreMinimal.h"
#include "Objects/InteratableObjectBase.h"
#include "FCDoor.generated.h"

class UTimelineComponent;
class UCurveFloat;

UCLASS()
class FABRICATION_API AFCDoor : public AInteratableObjectBase
{
	GENERATED_BODY()
	
public:
	AFCDoor();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;

private:
	UFUNCTION()
	void OnRep_IsOpen();
	UFUNCTION()
	void HandleDoorProgress(float Value);
	UFUNCTION()
	void OnTimelineFinished();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DoorTimeline;
	UPROPERTY(EditDefaultsOnly, Category = "Door")
	TObjectPtr<UCurveFloat> DoorCurve;
	UPROPERTY(EditAnywhere, Category = "Door", meta = (ClampMin = "-120.0", ClampMax = "120.0"))
	float TargetYaw;
	UPROPERTY(ReplicatedUsing = OnRep_IsOpen)
	uint8 bIsOpen : 1;
	FRotator InitialRotation;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorFrame;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> InteractSpot;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> OpenSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> CloseSound;
};
