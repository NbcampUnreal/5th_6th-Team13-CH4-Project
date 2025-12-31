#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController_Loading.generated.h"

class UFC_LoadingWidget;

UCLASS()
class FABRICATION_API AFCPlayerController_Loading : public APlayerController
{
	GENERATED_BODY()

public:
	AFCPlayerController_Loading();

protected:
	virtual void BeginPlay() override;

public:
	// 로딩 진행률 표시용 Client RPC
	UFUNCTION(Client, Reliable)
	void ClientRPCShowLoadingProgress(float Progress);

private:
	// 로딩 UI 블루프린트 위젯 클래스 (에디터에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Loading UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UFC_LoadingWidget> LoadingWidgetClass;

	// 로딩 UI 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UFC_LoadingWidget> LoadingWidget;
};

