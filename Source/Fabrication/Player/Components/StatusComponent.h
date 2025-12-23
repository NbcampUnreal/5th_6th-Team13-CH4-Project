// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChanagedDelegate, int32);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FABRICATION_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UStatusComponent Override

public:
	UStatusComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

#pragma region Var
public:
	FOnCurrentHPChanagedDelegate OnCurrentHPChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	int32 MaxHP;
#pragma endregion

#pragma region Replicated Var
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_ChangeHP)
	int32 CurrentHP;
#pragma endregion

#pragma region Function
public:
	UFUNCTION()
	void OnRep_ChangeHP();

	int32 ApplyDamage(int32 InDamage);

	UFUNCTION()
	void HealHP(int32 Heal);
#pragma endregion

#pragma region Getter/Setter

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetCurrentHP() const { return CurrentHP; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentHP(int32 InCurHP);

	FORCEINLINE int32 GetMaxHP() const { return MaxHP; }
	void SetMaxHP(int32 InMaxHP);
#pragma endregion



};
