#pragma once

#include "CoreMinimal.h"
#include "Objects/InteratableObjectBase.h"
#include "EscapeDoor.generated.h"

class UTimelineComponent;

UCLASS()
class FABRICATION_API AEscapeDoor : public AInteratableObjectBase
{
	GENERATED_BODY()
	
public:
	AEscapeDoor();
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
	void OpenDoor();
	void CloseDoor();
	void UpdateWidget(bool bCanEscape);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DoorTimeline;
	UPROPERTY(EditAnywhere, Category = "Door")
	TObjectPtr<UCurveFloat> DoorCurve;
	UPROPERTY(EditAnywhere, Category = "Door", meta = (ClampMin = "-120.0", ClampMax = "120.0"))
	float TargetYaw;
	UPROPERTY(ReplicatedUsing = OnRep_IsOpen)
	uint8 bIsOpen : 1;
	FRotator InitialRotation;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UTexture2D> LockDoorImage;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UTexture2D> UnLockDoorImage;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> OpenSound;
};
