#include "Objects/FCClock.h"
#include "Components/BoxComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Player/FCPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Fabrication.h"

AFCClock::AFCClock()
	: InteractSpot(nullptr)
	, NoiseSound(nullptr)
	, Loudness(1.0f)
	, PlayerSoundCount(0)
	, NoiseLocation()
{
	InteractSpot = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractSpot"));
	InteractSpot->SetupAttachment(SceneComp);
	InteractSpot->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractSpot->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractSpot->SetCollisionResponseToChannel(ECC_PickUp, ECR_Block);

}

void AFCClock::BeginPlay()
{
	Super::BeginPlay();

	NoiseLocation = GetActorLocation();
}

void AFCClock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerSoundCount);
}

void AFCClock::Interact(ACharacter* User, const FHitResult& HitResult)
{
	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(User))
	{
		Player->ServerRPCInteract(this, User, HitResult);
	}
}

void AFCClock::ExecuteServerLogic(ACharacter* User, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	ensureMsgf(NoiseSound, TEXT("NoiseSound 가 유효하지 않습니다. [%s]"), *GetName());

	MakeNoise();
}

void AFCClock::MakeNoise()
{
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		NoiseLocation,
		Loudness,
		this,
		0.0f,  // MaxRange (0 = 무제한, Perception에서 처리)
		FName("Lure")  // 태그: Lure
	);
	
	PlayerSoundCount++;

#if WITH_EDITOR
	if (bShowDebug)
	{
		// 초록색 구체: 테스트 Lure 소리
		DrawDebugSphere(GetWorld(), NoiseLocation, 120.0f, 16, FColor::Green, false, 4.0f, 0, 4.0f);
		DrawDebugString(GetWorld(), NoiseLocation + FVector(0, 0, 150),
			FString::Printf(TEXT("TEST LURE! (%.1f)"), Loudness), nullptr, FColor::Green, 4.0f);
	}
#endif
}

void AFCClock::OnRep_PlaySound()
{
	if (!IsValid(NoiseSound)) return;

	UGameplayStatics::PlaySoundAtLocation(this, NoiseSound, NoiseLocation);
}
