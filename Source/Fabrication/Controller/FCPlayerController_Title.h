#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController_Title.generated.h"

class UUserWidget;

UCLASS()
class FABRICATION_API AFCPlayerController_Title : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	void JoinServer(const FString& InIPAddress);
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> TitleWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUserWidget> TitleWidgetInstance;
};
