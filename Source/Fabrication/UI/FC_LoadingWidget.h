#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FC_LoadingWidget.generated.h"

UCLASS()
class FABRICATION_API UFC_LoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 로딩 진행률 업데이트 함수 (블루프린트에서도 호출 가능)
	UFUNCTION(BlueprintImplementableEvent, Category = "Loading")
	void UpdateProgress(float Progress);

	// C++에서도 호출 가능한 함수
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void SetProgress(float Progress);
};

