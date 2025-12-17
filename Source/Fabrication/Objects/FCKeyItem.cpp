#include "Objects/FCKeyItem.h"
#include "Data/FCKeyDataAsset.h"
#include "Data/FCKeyType.h"
#include "GameState/FCGameState.h"

AFCKeyItem::AFCKeyItem()
{
}

void AFCKeyItem::Interact(ACharacter* User, const FHitResult& HitResult)
{
	ServerRPCAddKey();
	//Super::Interact(User, HitResult);
}

void AFCKeyItem::InitializeKey(const UFCKeyDataAsset* InKeyData)
{
	if (!IsValid(InKeyData)) return;

	KeyID = InKeyData->GetKeyID();
	DisplayName = InKeyData->GetDisplayName();
}

void AFCKeyItem::ServerRPCAddKey_Implementation()
{
	AFCGameState* GS = GetWorld()->GetGameState<AFCGameState>();
	if (IsValid(GS))
	{
		GS->SetKeyCollected();
		//Destroy();
	}
}
