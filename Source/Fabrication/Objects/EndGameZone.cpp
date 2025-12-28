#include "Objects/EndGameZone.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Player/FCPlayerCharacter.h"
#include "GameState/FCGameState.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#include "EngineUtils.h"
#endif
#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#include "Components/TextRenderComponent.h"
#endif

#pragma region Construct

AEndGameZone::AEndGameZone()
	: SceneComp(nullptr)
	, EndPlayZone(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComp);

	EndPlayZone = CreateDefaultSubobject<UBoxComponent>(TEXT("EndPlayZone"));
	EndPlayZone->SetupAttachment(SceneComp);

#if WITH_EDITORONLY_DATA
	if (IsValid(SceneComp))
	{
		EditorVisualizer = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("EditorVisualizer"));
		if (IsValid(EditorVisualizer))
		{
			EditorVisualizer->SetupAttachment(SceneComp);
			EditorVisualizer->SetHiddenInGame(true);
			EditorVisualizer->bIsEditorOnly = true;

			InfoText = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("EditorOnlyInfoText"));
			if (IsValid(InfoText))
			{
				InfoText->SetupAttachment(EditorVisualizer);
				InfoText->SetText(FText::FromString(TEXT("End Zone")));
				InfoText->SetHiddenInGame(true);
				InfoText->bIsEditorOnly = true;
			}
		}
	}
#endif

}

void AEndGameZone::BeginPlay()
{
	Super::BeginPlay();
	
	if (!EndPlayZone->OnComponentBeginOverlap.IsAlreadyBound(this, &AEndGameZone::OnPlayerOverlap))
	{
		EndPlayZone->OnComponentBeginOverlap.AddDynamic(this, &AEndGameZone::OnPlayerOverlap);
	}
	if (!EndPlayZone->OnComponentEndOverlap.IsAlreadyBound(this, &AEndGameZone::OnPlayerEndOverlap))
	{
		EndPlayZone->OnComponentEndOverlap.AddDynamic(this, &AEndGameZone::OnPlayerEndOverlap);
	}

}

void AEndGameZone::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor))
	{
		AFCGameState* GS = GetWorld()->GetGameState<AFCGameState>();
		if (IsValid(GS))
		{
			GS->MatchState = EMatchState::Ending;
		}
	}
}

void AEndGameZone::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(OtherActor))
	{
		// 필요하면 로직 추가
	}
}

#pragma endregion

#pragma region EditorOnly
#if WITH_EDITOR
EDataValidationResult AEndGameZone::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	UWorld* World = GetWorld();
	if (!IsValid(World)) return Result;

	int32 ActorCount = 0;
	
	for (TActorIterator<AEndGameZone> It(World); It; ++It)
	{
		ActorCount++;
		if (ActorCount > 1)
		{
			Context.AddError(FText::FromString(TEXT("EndGameZone 액터는 레벨에 단 하나만 있어야 합니다.")));
			Result = EDataValidationResult::Invalid;
			break;
		}
	}

	return Result;
}
#endif
#pragma endregion