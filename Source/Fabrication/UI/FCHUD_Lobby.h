#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCHUD_Lobby.generated.h"

class UFCChatting_Lobby;
class UFCRoomList_Lobby;
class UFCRoomWaiting_Lobby;

UCLASS()
class FABRICATION_API UFCHUD_Lobby : public UUserWidget
{
	GENERATED_BODY()

public:
	template <typename T>
	T* GetWidget() const
	{
		static_assert(TIsDerivedFrom<T, UUserWidget>::IsDerived, "T Must be derived from UUserWidget");

		for (UUserWidget* Widget : ManagedWidgets)
		{
			if (!IsValid(Widget))
			{
				continue;
			}

			if (T* CastedWidget = Cast<T>(Widget))
			{
				return CastedWidget;
			}
		}

		return nullptr;
	}

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFCChatting_Lobby> Chatting;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFCRoomList_Lobby> RoomList;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFCRoomWaiting_Lobby> RoomWaiting;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> ManagedWidgets;
};
