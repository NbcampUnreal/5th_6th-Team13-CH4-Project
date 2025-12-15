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
	void SetPlayerNickName(const FString& InNickName);
	void JoinServer(const FString& InIPAddress, const FString& InNickName);
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> TitleWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> TitleWidgetInstance;
};
