
#include "Player/Components/SpeedControlComponent.h"
#include "Player/FCPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	// ...
	
}

void USpeedControlComponent::SetSpeedByHealth(int32 CurHP)
{
	if (IsValid(OwnerCharacterMoveComp))
	{
		OwnerCharacterMoveComp->MaxWalkSpeed = SpeedChangeValueArray[CurHP];
	}
}

