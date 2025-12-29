#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FCGameMode_Loading.generated.h"

UCLASS()
class FABRICATION_API AFCGameMode_Loading : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFCGameMode_Loading();

	virtual void BeginPlay() override;

	// 게임 맵으로 이동하는 함수
	void TravelToGameMap();

	// 로딩 맵에서 게임 맵으로 이동하기 전 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float LoadingDelay = 2.0f;

private:
	// URL에서 읽은 다음 맵 경로
	FString NextMapPath;

	// 타이머 핸들
	FTimerHandle TravelTimerHandle;
	FTimerHandle ProgressUpdateTimerHandle;

	// 현재 로딩 진행률 (0.0 ~ 1.0)
	float CurrentProgress = 0.0f;

	// URL에서 NextMap 파라미터를 읽어오는 함수
	void ParseNextMapFromURL();

	// 로딩 진행률 업데이트 함수
	void UpdateLoadingProgress();

	// 모든 플레이어에게 로딩 진행률을 전달하는 함수
	void BroadcastLoadingProgress(float Progress);
};

