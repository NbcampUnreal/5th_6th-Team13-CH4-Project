#include "UI/FCTitleLayout.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Controller/FCPlayerController_Title.h"

UFCTitleLayout::UFCTitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFCTitleLayout::NativeConstruct()
{
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UFCTitleLayout::OnPlayButtonClicked()
{
	AFCPlayerController_Title* TitlePlayerController = GetOwningPlayer<AFCPlayerController_Title>();
	if (!IsValid(TitlePlayerController)) return;

	FString ServerIP = ServerIPEditableText->GetText().ToString();
	FString PlayerNickName = PlayerNickNameEditableText->GetText().ToString();
	TitlePlayerController->JoinServer(ServerIP, PlayerNickName);
}

void UFCTitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
