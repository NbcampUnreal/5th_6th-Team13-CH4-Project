#include "Items/NoiseItem.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Fabrication.h"
#include "DrawDebugHelpers.h"
#include "Monster/FCSoundHunter.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ANoiseItem::ANoiseItem()
{
	ItemID = TEXT("NoiseItem");
	bReplicates = true; 
	PrimaryActorTick.bCanEverTick = false;

	//AI Perception Stimuli Source ������Ʈ �߰� 
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	//Hearing Sense ��� (Sight ����)
	StimuliSource->bAutoRegister = true; 
}

void ANoiseItem::BeginPlay()
{
	Super::BeginPlay();

	if (StaticMeshComp)
	{
		StaticMeshComp->SetVisibility(true);
		StaticMeshComp->SetHiddenInGame(false);
	}
	if (InteractableWidget)
	{
		InteractableWidget->SetVisibility(true);
	}
	if (HasAuthority())
	{
		if (BoxComp)
		{
			BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (StimuliSource)
		{
			StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());
			StimuliSource->RegisterWithPerceptionSystem();
			FC_LOG_NET(LogFCNet, Log, TEXT("[NoiseItem] StimuliSource registered for Hearing"));
		}
	}
}

void ANoiseItem::ActivateNoise()
{
	if (!HasAuthority()) return;

	FC_LOG_NET(LogFCNet, Warning, TEXT("[NoiseItem] ActivateNoise called - Delay: %.1fs, Repeat: %s"),
		ActivationDelay, bRepeatNoise ? TEXT("true") : TEXT("false"));

	if (bRepeatNoise)
	{
		GetWorldTimerManager().SetTimer(
			NoiseTimerHandle,
			this,
			&ANoiseItem::TriggerNoise,
			RepeatInterval,
			true,//Loop 
			ActivationDelay
		);
	}
	else
	{
		//1ȸ�� 
		GetWorldTimerManager().SetTimer(
			NoiseTimerHandle,
			this,
			&ANoiseItem::TriggerNoise,
			ActivationDelay,
			false
		);
	}

	if (Duration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			DurationTimerHandle,
			[this]()
			{
				GetWorldTimerManager().ClearTimer(NoiseTimerHandle);
				Destroy();
			},
			Duration,
			false
		);
	}
}

void ANoiseItem::TriggerNoise()
{
	const FVector NoiseLocation = GetActorLocation(); //�Ҹ� �߻� ��ġ = �� ��ġ 

	const FName NoiseTag = bUseLureTag ? FName("Lure") : NAME_None; //AI Perception�� �Ҹ� ��ȣ ���� 

	FC_LOG_NET(LogFCNet, Warning, TEXT("[NoiseItem] === TriggerNoise START ==="));
	FC_LOG_NET(LogFCNet, Warning, TEXT("  Location: %s"), *NoiseLocation.ToString());
	FC_LOG_NET(LogFCNet, Warning, TEXT("  Tag: %s"), *NoiseTag.ToString());
	FC_LOG_NET(LogFCNet, Warning, TEXT("  Loudness: %.2f"), Loudness);

	if (NoiseSound)
	{
		Multicast_PlayNoiseSound(NoiseLocation);
	}

	UAISense_Hearing::ReportNoiseEvent( 
		GetWorld(),
		NoiseLocation,
		Loudness,
		this,
		0.0f,    //MaxRange(0.0~1.0f) (0.0f = ������ | AI HearingRadius���� ó��)
		NoiseTag //"Lure or NAME_None"
	);

	FC_LOG_NET(LogFCNet, Warning, TEXT("[NoiseItem] Noise Triggered Location:%s . Tag:%s"), 
		*NoiseLocation.ToString(), *NoiseTag.ToString());

#if WITH_EDITOR
	FColor DebugColor = bUseLureTag ? FColor::Green : FColor::Yellow; 
	DrawDebugSphere(GetWorld(), NoiseLocation, 150.0f, 16, DebugColor, false, 5.0f, 0, 5.0f);

	DrawDebugString(GetWorld(), NoiseLocation + FVector(0, 0, 150.0f),
		FString::Printf(TEXT("Noise! Tag:%s (%.1f)"),*NoiseTag.ToString(), Loudness), 
		nullptr, DebugColor, 5.0f, true, 2.0f);

	DrawDebugLine(GetWorld(), NoiseLocation, NoiseLocation + FVector(0, 0, 500.0f), DebugColor, false, 5.0f, 0, 5.0f);

	DrawDebugCircle(GetWorld(), NoiseLocation, 3000.0f, 64, FColor::Cyan, false, 5.0f, 0, 2.0f, FVector(1, 0, 0), FVector(0, 1, 0), false);
#endif
}

void ANoiseItem::Multicast_PlayNoiseSound_Implementation(FVector Loc) 
{
	if (NoiseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			NoiseSound,
			Loc,
			FRotator::ZeroRotator,
			1.0f,
			1.0f,
			0.0f,
			NoiseSoundAttenuation
		);
	}
}
