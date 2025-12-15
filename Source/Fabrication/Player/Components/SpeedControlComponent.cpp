
#include "Player/Components/SpeedControlComponent.h"
#include "Player/FCPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Components/StatusComponent.h"

USpeedControlComponent::USpeedControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	SpeedChangeValueArray = {350.0f, 500.0f, 600.0f};
	OwnerCharacter = Cast<AFCPlayerCharacter>(GetOwner());

	if (IsValid(OwnerCharacter))
	{
		OwnerCharacterMoveComp = OwnerCharacter->GetCharacterMovement();
	}
}


// Called when the game starts
void USpeedControlComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AFCPlayerCharacter* FCPlayerCharacter = Cast<AFCPlayerCharacter>(GetOwner()))
	{
		UStatusComponent* StatusComp = FCPlayerCharacter->StatusComp;

		StatusComp->OnCurrentHPChanged.AddUObject(this, &USpeedControlComponent::SetSpeedByHealth);
	}

}

void USpeedControlComponent::SetSpeedByHealth(int32 CurHP)
{
	if (IsValid(OwnerCharacterMoveComp) && SpeedChangeValueArray.IsValidIndex(CurHP - 1))
	{
		OwnerCharacterMoveComp->MaxWalkSpeed = SpeedChangeValueArray[CurHP - 1];
	}
}

