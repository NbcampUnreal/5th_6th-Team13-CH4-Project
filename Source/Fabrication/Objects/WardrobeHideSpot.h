#pragma once

#include "CoreMinimal.h"
#include "Objects/InteratableObjectBase.h"
#include "WardrobeHideSpot.generated.h"

class UBoxComponent;
class UTimelineComponent;

UCLASS()
class FABRICATION_API AWardrobeHideSpot : public AInteratableObjectBase
{
	GENERATED_BODY()
	
public:
	AWardrobeHideSpot();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;

private:
	UFUNCTION()
	void OnRep_IsOpen();
	UFUNCTION()
	void HandleDoorProgress(float Value);
	UFUNCTION()
	void OnTimelineFinished();
	void OpenDoor();
	void CloseDoor();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Door;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> InteractSpot;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DoorTimeline;
	UPROPERTY(EditAnywhere, Category = "Door")
	TObjectPtr<UCurveFloat> DoorCurve;
	UPROPERTY(EditAnywhere, Category = "Door", meta = (ClampMin = "-120.0", ClampMax = "120.0"))
	float TargetYaw;
	UPROPERTY(ReplicatedUsing = OnRep_IsOpen)
	uint8 bIsOpen : 1;
	FRotator InitialRotation;
	
};
