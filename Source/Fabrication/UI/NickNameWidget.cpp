// Fill out your copyright notice in the Description page of Project Settings.
#include "NickNameWidget.h"
#include "Components/TextBlock.h"
#include "Player/FCPlayerCharacter.h"
#include "PlayerState/FCPlayerState.h"

void UNickNameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AFCPlayerCharacter* Owner = Cast<AFCPlayerCharacter>(GetOwningPlayer()->GetPawn());
	
	if (IsValid(Owner))
	{
		if (AFCPlayerState* PlayerState = Owner->GetPlayerState<AFCPlayerState>())
		{
			SetPlayerNickName(PlayerState->GetPlayerNickName());
		}
	}
}

void UNickNameWidget::SetPlayerNickName(const FString& PlayerNickName)
{
	PlayerNickNameText->SetText(FText::FromString(PlayerNickName));
}


