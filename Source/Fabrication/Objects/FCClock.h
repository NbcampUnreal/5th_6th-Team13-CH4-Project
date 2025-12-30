#pragma once

#include "CoreMinimal.h"
#include "Objects/InteratableObjectBase.h"
#include "FCClock.generated.h"

class UBoxComponent;

UCLASS()
class FABRICATION_API AFCClock : public AInteratableObjectBase
{
	GENERATED_BODY()

public:
	AFCClock();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Interact(ACharacter* User, const FHitResult& HitResult) override;
	virtual void ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult) override;

private:
	void MakeNoise();
	UFUNCTION()
	void OnRep_PlaySound();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> InteractSpot;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> NoiseSound;
	// Noise 크기 (0.0 ~ 1.0)
	UPROPERTY(EditDefaultsOnly, Category = "Noise", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float Loudness;
	UPROPERTY(ReplicatedUsing = OnRep_PlaySound)
	int32 PlayerSoundCount;
	FVector NoiseLocation;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool bShowDebug = true;
#endif
};
