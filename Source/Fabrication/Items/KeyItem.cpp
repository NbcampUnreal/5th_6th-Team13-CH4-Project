#include "Items/KeyItem.h"
#include "GameState/FCGameState.h"
#include "Player/FCPlayerCharacter.h"

AKeyItem::AKeyItem()
{
}

void AKeyItem::Interact(ACharacter* User, const FHitResult& HitResult)
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
	if (IsValid(Player))
	{
		Player->ServerRPCInteract(this, User, HitResult);
	}
}

void AKeyItem::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	AFCGameState* GS = GetWorld()->GetGameState<AFCGameState>();
	if (IsValid(GS))
	{
		GS->SetKeyCollected();
		Destroy();
	}
}
