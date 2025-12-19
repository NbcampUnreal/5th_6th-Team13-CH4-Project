#include "Animation/AnimNotify_FlashLightOff.h"
#include "Player/FCPlayerCharacter.h"

void UAnimNotify_FlashLightOff::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(MeshComp->GetOwner());
	if (!Player) return;
	//Client -> Server RPC ¿äÃ» 
	if (Player->IsLocallyControlled())
	{
		Player->ServerRPCChangeUseFlashLightValue(false);
	}
}
