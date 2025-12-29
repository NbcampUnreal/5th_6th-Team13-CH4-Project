#include "Controller/FCPlayerController_Loading.h"
#include "UI/FC_LoadingWidget.h"

AFCPlayerController_Loading::AFCPlayerController_Loading()
	: LoadingWidgetClass(nullptr)
	, LoadingWidget(nullptr)
{
}

void AFCPlayerController_Loading::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		// 마우스 커서 표시
		bShowMouseCursor = true;
		
		// UI 전용 입력 모드
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		// 로딩 UI 생성
		if (LoadingWidgetClass)
		{
			LoadingWidget = CreateWidget<UFC_LoadingWidget>(this, LoadingWidgetClass);
			if (LoadingWidget)
			{
				LoadingWidget->AddToViewport();
			}
		}
	}
}

void AFCPlayerController_Loading::ClientRPCShowLoadingProgress_Implementation(float Progress)
{
	if (!IsLocalController()) return;
	if (!LoadingWidget) return;

	// 로딩 위젯의 진행률 업데이트 함수 호출
	LoadingWidget->SetProgress(Progress);
}

