#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCTimerWidget.generated.h"

class UTextBlock;
class AFCGameState;

UCLASS()
class FABRICATION_API UFCTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 타이머 텍스트 업데이트 함수
	void UpdateTimerText(int32 RemainTime);

protected:
	// 타이머 업데이트 함수 (주기적으로 호출됨)
	UFUNCTION()
	void UpdateTimer();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;

private:
	// GameState 참조
	UPROPERTY()
	TObjectPtr<AFCGameState> GameState;

	// 타이머 핸들 (주기적으로 GameState를 체크)
	FTimerHandle TimerUpdateHandle;

	// 이전 RemainGameTime 값 (변경 감지용)
	int32 LastRemainGameTime = -1;
};
