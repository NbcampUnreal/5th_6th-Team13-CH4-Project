// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NickNameWidget.generated.h"

/**
 * 
 */
class UTextBlock;
UCLASS()
class FABRICATION_API UNickNameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(bindWidget))
	TObjectPtr<UTextBlock> PlayerNickNameText;
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerNickName(const FString& PlayerNickName);
	
	UFUNCTION(BlueprintCallable)
	void NativeConstruct() override;
};
