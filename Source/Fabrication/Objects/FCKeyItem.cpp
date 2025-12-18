#include "Objects/FCKeyItem.h"
#include "Data/FCKeyDataAsset.h"
#include "Data/FCKeyType.h"
#include "GameState/FCGameState.h"
#include "Player/FCPlayerCharacter.h"

AFCKeyItem::AFCKeyItem()
{
}

void AFCKeyItem::Interact(ACharacter* User, const FHitResult& HitResult)
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User);
	if (IsValid(Player))
	{
		Player->ServerRPCInteract(this, User, HitResult);
	}
}

void AFCKeyItem::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	AFCGameState* GS = GetWorld()->GetGameState<AFCGameState>();
	if (IsValid(GS))
	{
		GS->SetKeyCollected();
		Destroy();
	}
}