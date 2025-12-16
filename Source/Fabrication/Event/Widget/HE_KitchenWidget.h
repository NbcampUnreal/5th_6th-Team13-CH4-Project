// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HE_KitchenWidget.generated.h"

/**
 * 
 */
UCLASS()
class FABRICATION_API UHE_KitchenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void ShowInteract();

    UFUNCTION(BlueprintImplementableEvent)
    void HideInteract();

    UFUNCTION(BlueprintImplementableEvent)
    void UpdateInteractText(const FText& NewText);
};
