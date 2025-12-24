#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnZone.generated.h"

class USceneComponent;
class UBoxComponent;

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
