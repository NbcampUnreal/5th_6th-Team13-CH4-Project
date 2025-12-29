#include "GameState/UI/FC_SharedNote.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "GameState/FCGameState.h"
#include "Engine/DataTable.h"
#include "Items/Data/NoteData.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"

static UWidget* FindChildWidgetByName(UWidget* Root, const FName& TargetName);

void UFC_SharedNote::NativeConstruct()
{
    Super::NativeConstruct();

    GridNoteItems.Add(NoteItem1);
    GridNoteItems.Add(NoteItem2);
    GridNoteItems.Add(NoteItem3);
    GridNoteItems.Add(NoteItem4);
    GridNoteItems.Add(NoteItem5);
    GridNoteItems.Add(NoteItem6);
    GridNoteItems.Add(NoteItem7);
    GridNoteItems.Add(NoteItem8);
    GridNoteItems.Add(NoteItem9);
    GridNoteItems.Add(NoteItem10);
    GridNoteItems.Add(NoteItem11);
    GridNoteItems.Add(NoteItem12);
    GridNoteItems.Add(NoteItem13);
    GridNoteItems.Add(NoteItem14);

    NoteListItems.Add(NoteListItem_1);
    NoteListItems.Add(NoteListItem_2);
    NoteListItems.Add(NoteListItem_3);
    NoteListItems.Add(NoteListItem_4);
    NoteListItems.Add(NoteListItem_5);
    NoteListItems.Add(NoteListItem_6);
    NoteListItems.Add(NoteListItem_7);
    NoteListItems.Add(NoteListItem_8);
    NoteListItems.Add(NoteListItem_9);
    NoteListItems.Add(NoteListItem_10);
    NoteListItems.Add(NoteListItem_11);
    NoteListItems.Add(NoteListItem_12);
    NoteListItems.Add(NoteListItem_13);
    NoteListItems.Add(NoteListItem_14);

    for (UBorder* Item : NoteListItems)
    {
        if (Item) {
            Item->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    LoadNotesFromGameState();
}

void UFC_SharedNote::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GridViewTabButton)
        GridViewTabButton->OnClicked.AddUniqueDynamic(this, &UFC_SharedNote::OnGridViewButtonClicked);

    if (ListViewTabButton)
        ListViewTabButton->OnClicked.AddUniqueDynamic(this, &UFC_SharedNote::OnListViewButtonClicked);
}

void UFC_SharedNote::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!GetWorld()) return;

    // GameState의 노트 개수 변경 감지
    if (AFCGameState* GS = GetWorld()->GetGameState<AFCGameState>())
    {
        const int32 CurrNoteCount = GS->CollectedNoteIDs.Num();
        if (CurrNoteCount != LastKnownNoteCount)
        {
            LoadNotesFromGameState();
            LastKnownNoteCount = CurrNoteCount;
        }
    }
}

void UFC_SharedNote::OnGridViewButtonClicked()
{
    if (ContentSwitcher)
    {
        ContentSwitcher->SetActiveWidgetIndex(0);
    }
    if (GridViewTabButton)
    {
        GridViewTabButton->SetIsEnabled(false);
    }
    if (ListViewTabButton)
    {
        ListViewTabButton->SetIsEnabled(true);
    }
}

void UFC_SharedNote::OnListViewButtonClicked()
{
    if (ContentSwitcher)
    {
        ContentSwitcher->SetActiveWidgetIndex(1);
    }
    if (GridViewTabButton)
    {
        GridViewTabButton->SetIsEnabled(true);
    }
    if (ListViewTabButton)
    {
        ListViewTabButton->SetIsEnabled(false);
    }

    RefreshListView();
}

void UFC_SharedNote::UpdateNoteItemState(int32 NoteID, bool bIsCollected)
{
    if (NoteID < 1 || NoteID > GridNoteItems.Num()) return;

    //Grid View의 노트 아이템 상태 업데이트 
    UBorder* GridItem = GetGridNoteItemByIndex(NoteID - 1);
    if (GridItem)
    {
        UpdateGridNoteItem(GridItem, NoteID, bIsCollected);
    }
}

void UFC_SharedNote::UpdateCollectedCount(int32 Count)
{
    if (StateCollectedValue)
    {
        StateCollectedValue->SetText(FText::AsNumber(Count));
    }
    if (StateRemainingValue)
    {
        const int32 Remaining = 14 - Count;
        StateRemainingValue->SetText(FText::AsNumber(Remaining));
    }
}

void UFC_SharedNote::RefreshListView()
{
    //모든 아이템 숨김 
    for (UBorder* Item : NoteListItems)
    {
        if (Item)
        {
            Item->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    //데이터 재 로드 
    LoadNotesFromGameState();
}

UBorder* UFC_SharedNote::GetGridNoteItemByIndex(int32 Index)
{
    if (Index >= 0 && Index < GridNoteItems.Num())
    {
        return GridNoteItems[Index];
    }
    return nullptr;
}

UBorder* UFC_SharedNote::GetNoteListItemByIndex(int32 Index)
{
    if (Index >= 0 && Index < NoteListItems.Num()) 
    {
        return NoteListItems[Index];
    }
    return nullptr;
}

void UFC_SharedNote::UpdateNoteListItem(UBorder* Item, int32 NoteID, const FText& Content, int32 ListSlotIndex)
{
    if (!Item) return; 

    const int32 Suffix = ListSlotIndex + 1; 

    const FName NumberName(*FString::Printf(TEXT("ListItemNumber_%d"), Suffix));
    const FName TextName(*FString::Printf(TEXT("ListItemText_%d"), Suffix));

    //Note Num & Note Content
    if (UWidget* RootChild = Item->GetContent())
    {
        if (UTextBlock* IDText = Cast<UTextBlock>(FindChildWidgetByName(RootChild, NumberName)))
        {
            IDText->SetText(FText::AsNumber(NoteID));
        }

        if (UTextBlock* ContentText = Cast<UTextBlock>(FindChildWidgetByName(RootChild, TextName)))
        {
            ContentText->SetText(Content);
        }
    }
}

void UFC_SharedNote::UpdateGridNoteItem(UBorder* Item, int32 NoteID, bool bIsCollected)
{
    if (!Item) return;

    if (UWidget* RootChild = Item->GetContent())
    {
        if (UTextBlock* NumberText = Cast<UTextBlock>(FindChildWidgetByName(RootChild, TEXT("Text_NoteNumber"))))
        {
            NumberText->SetText(FText::AsNumber(NoteID));
        }
    }
}

static UWidget* FindChildWidgetByName(UWidget* Root, const FName& TargetName)
{
    if (!Root) return nullptr;
    if (Root->GetFName() == TargetName) return Root;

    if (UPanelWidget* Panel = Cast<UPanelWidget>(Root))
    {
        const int32 Count = Panel->GetChildrenCount();
        for (int32 i = 0; i < Count; ++i)
        {
            if (UWidget* Found = FindChildWidgetByName(Panel->GetChildAt(i), TargetName))
            {
                return Found;
            }
        }
    }

    return nullptr;
}

void UFC_SharedNote::LoadNotesFromGameState()
{
    if (!NoteDataTable) return;
    if (!GetWorld()) return;

    AFCGameState* GS = GetWorld()->GetGameState<AFCGameState>();
    if (!GS) return;

    const TArray<int32>& CollectedNotes = GS->CollectedNoteIDs;


    //수집 카운트 업데이트
    UpdateCollectedCount(CollectedNotes.Num());

    //각 수집된 노트에 대해 UI 업데이트
    for (int32 i = 0; i < CollectedNotes.Num(); ++i)
    {
        const int32 NoteID = CollectedNotes[i];

        if (NoteID < 1 || NoteID > 14) continue;

        if (!GridNoteItems.IsValidIndex(NoteID - 1)) continue;
        if (!NoteListItems.IsValidIndex(i)) continue;

        FName RowName = FName(*FString::Printf(TEXT("Note_%d"), NoteID - 1));
        FNoteData* NoteData = NoteDataTable->FindRow<FNoteData>(RowName, TEXT(""));

        if (!NoteData) continue;

        //Grid View 업데이트 
        UpdateNoteItemState(NoteID, true);

        if (UBorder* Item = NoteListItems[i])
        {
            UpdateNoteListItem(Item, NoteID, NoteData->NoteText, i);
            Item->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void UFC_SharedNote::PlayShow()
{
    SetVisibility(ESlateVisibility::Visible);

    if (MainFrame)
    {
        MainFrame->SetRenderOpacity(0.f);
    }

    if (ShowNote) PlayAnimation(ShowNote);
}

void UFC_SharedNote::PlayHide()
{
    if (HideNote) PlayAnimation(HideNote);
}