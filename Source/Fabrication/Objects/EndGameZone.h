#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndGameZone.generated.h"

class USceneComponent;
class UBoxComponent;

UCLASS()
class FABRICATION_API AEndGameZone : public AActor
{
	GENERATED_BODY()

#pragma region Construct

public:	
	AEndGameZone();

protected:
	virtual void BeginPlay() override;

#pragma endregion

protected:
	UFUNCTION()
	void OnPlayerOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnPlayerEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> EndPlayZone;

#pragma region EditorOnly
#if WITH_EDITOR
protected:
	// EditorOnly
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<class UBillboardComponent> EditorVisualizer;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<class UTextRenderComponent> InfoText;
#endif
#pragma endregion

};
