#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnZone.generated.h"

class USceneComponent;
class UBoxComponent;
class UBillboardComponent;
class UTextRenderComponent;

UCLASS()
class FABRICATION_API ASpawnZone : public AActor
{
	GENERATED_BODY()
	
#pragma region 생성
public:	
	ASpawnZone();
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Editor Option
private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UBillboardComponent> EditorVisualizer;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UTextRenderComponent> InfoText;

#pragma endregion

public:
	UFUNCTION(BlueprintCallable, Category = "Test")
	AActor* SpawnActorInZone(TSubclassOf<AActor> InActor);

private:
	FVector GetRandomLocation();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> SpawnArea;
};
