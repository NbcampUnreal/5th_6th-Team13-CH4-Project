#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "Components/Border.h"
#include "FC_SharedNote.generated.h"

UCLASS()
class FABRICATION_API UFC_SharedNote : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

#pragma region Widget Component
public:
    UPROPERTY(meta=(BindWidget))
    UButton* GridViewTabButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ListViewTabButton;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* ContentSwitcher;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ListScrollBox;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* NoteListVerticalBox;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StateCollectedValue;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StateRemainingValue;

    UPROPERTY(meta=(BindWidget))
    UBorder* MainFrame;

    //Grid View 
    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem1;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem2;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem3;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem4;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem5;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem6;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem7;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem8;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem9;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem10;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem11;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem12;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem13;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteItem14;
    //List View 
    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_1;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_2;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_3;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_4;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_5;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_6;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_7;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_8;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_9;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_10;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_11;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_12;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_13;

    UPROPERTY(meta = (BindWidget))
    UBorder* NoteListItem_14;

#pragma endregion

public:
    //Grid View Item State Update
    UFUNCTION(BlueprintCallable, Category="Shared Note")
    void UpdateNoteItemState(int32 NoteID, bool bIsCollected);

    //Add Note Update
    UFUNCTION(BlueprintCallable, Category = "Shared Note")
    void UpdateCollectedCount(int32 Count);

    //List View Update 
    UFUNCTION(BlueprintCallable, Category = "Shared Note")
    void RefreshListView();

    UFUNCTION(BlueprintCallable, Category = "Shared Note")
    void LoadNotesFromGameState();

    //NoteTable ÂüÁ¶ 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shared Note")
    UDataTable* NoteDataTable;

    UFUNCTION(BlueprintCallable)
    void PlayShow();

    UFUNCTION(BlueprintCallable)
    void PlayHide();

private:
    UFUNCTION()
    void OnGridViewButtonClicked();

    UFUNCTION()
    void OnListViewButtonClicked();

    TArray<UBorder*> GridNoteItems;
    TArray<UBorder*> NoteListItems;

    UBorder* GetNoteListItemByIndex(int32 Index);
    UBorder* GetGridNoteItemByIndex(int32 Index);

    void UpdateNoteListItem(UBorder* Item, int32 NoteID, const FText& Content, int32 ListSlotIndex);
    void UpdateGridNoteItem(UBorder* Item, int32 NoteID, bool bIsCollected);

    int32 LastKnownNoteCount = 0;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* ShowNote;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* HideNote;

};
