// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/BaseHazardEvent.h"
#include "HE_Painting.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class FABRICATION_API AHE_Painting : public ABaseHazardEvent
{
	GENERATED_BODY()
public :
	AHE_Painting();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetSpotLightIntensity(float NewIntensity);

	void SetPointLightColor(FLinearColor NewColor);
	
	void SetWatching(bool bIsWatching);

	void ToggleWatching();

	virtual void OnHazardRowReady() override;
	//UFUNCTION()
	//void OnLightOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	//	bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	bool bIsWatching = false;

protected:
	UPROPERTY(VisibleAnywhere)
	class USpotLightComponent* SpotLight;

	UPROPERTY(VisibleAnywhere)
	class UPointLightComponent* PointLight;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PaintingMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* PaintingMID;

	UPROPERTY(EditAnywhere, Category = "Painting|Texture")
	UTexture2D* EyeOpenTexture;

	UPROPERTY(EditAnywhere, Category = "Painting|Texture")
	UTexture2D* EyeClosedTexture;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* LightTriggerBox;

private:
	FTimerHandle WatchingTimerHandle;

	bool bIsCool;

	float TimeCheck = 0.0f;
};
