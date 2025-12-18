#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCRoomWaiting_Lobby.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class FABRICATION_API UFCRoomWaiting_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UTextBlock> RoomName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PlayButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ExitButton;
};
