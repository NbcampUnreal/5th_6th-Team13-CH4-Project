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
	
#pragma region Construct
public:	
	ASpawnZone();
protected:
	virtual void BeginPlay() override;
#pragma endregion

public:
	AActor* SpawnActorInZone(TSubclassOf<AActor> InActor);

private:
	FVector GetRandomLocation();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> SpawnArea;

#pragma region EditorOnly
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<class UBillboardComponent> EditorVisualizer;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<class UTextRenderComponent> InfoText;
#endif
#pragma endregion

};
