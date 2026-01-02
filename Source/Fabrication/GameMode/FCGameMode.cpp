#include "GameMode/FCGameMode.h"

#include "Controller/FCPlayerController.h"
#include "GameState/FCGameState.h"
#include "Event/LevelEventManager.h"
#include "PlayerState/FCPlayerState.h"
#include "Player/FCPlayerCharacter.h"//Add Item 테스트 용 
#include "Items/Inventory/FC_InventoryComponent.h"//Add Item 테스트 용 
#include "Objects/SpawnManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AFCGameMode::AFCGameMode()
	:	
	MinimumPlayerCountForPlaying(2),
	WaitingTime(5),
	RemainTimeForPlaying(WaitingTime),
	GameTimeLimit(5),
	bReadyForPlay(false),
	bAllPlayersReady(false),
	EndingTimeLimit(5),
	RemainEndingTime(EndingTimeLimit),
	GameRequireKey(0)
{
}

void AFCGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (ULevelEventManager* Manager = GetWorld()->GetSubsystem<ULevelEventManager>())
	{
		Manager->HazardDataTable = SetHazardDataTable;
		Manager->bInitialized = true;
	}
	
	ResetValues();
	
	GetWorldTimerManager().SetTimer(
		MainTimerHandle,
		this,
		&ThisClass::OnMainTimerElapsed,
		1.f,
		true);

	SpawnManager = GetSpawnManger();

	AFCGameState* GS = GetGameState<AFCGameState>();
	if (IsValid(GS))
	{
		GS->SetRequiredKey(GameRequireKey);
		GS->InitializeNote();
		GS->TotalGameTime = GameTimeLimit;
		GS->SetRemainWaitingTime(WaitingTime);
		
		// Waiting 상태 시작 시 타이머 위젯 표시
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AFCPlayerController* FCPC = Cast<AFCPlayerController>(*It);
			
			if (IsValid(FCPC))
			{
				FCPC->ClientRPCShowTimerWidget();
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle, 
		this, 
		&AFCGameMode::StartSpawn, 
		0.1f, 
		false);
}

void AFCGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AFCGameState* FCGS = GetGameState<AFCGameState>();
	if (!IsValid(FCGS))
	{
		return;
	}
	
	AFCPlayerController* FCPC = Cast<AFCPlayerController>(NewPlayer);
	if (IsValid(FCPC))
	{
		AlivePlayerControllers.AddUnique(FCPC);
		
		// Waiting 또는 Playing 상태일 때 타이머 위젯 표시
		if (FCGS->MatchState == EMatchState::Waiting || FCGS->MatchState == EMatchState::Playing)
		{
			FCPC->ClientRPCShowTimerWidget();
		}
	}
}

void AFCGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	AFCPlayerController* FCPC = Cast<AFCPlayerController>(Exiting);
	if (IsValid(FCPC))
	{
		AlivePlayerControllers.Remove(FCPC);
		DeadPlayerControllers.Remove(FCPC);
	}
}

void AFCGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	Super::EndPlay(EndPlayReason);
}

void AFCGameMode::PlayerDead(APlayerController* DeadPlayer)
{
	AFCPlayerController* DeadPC = Cast<AFCPlayerController>(DeadPlayer);
	if (!IsValid(DeadPC)) return;

 	const int32 DeadPlayerIndex = AlivePlayerControllers.Find(DeadPC);
	if (DeadPlayerIndex == INDEX_NONE) return;
	
	DeadPlayerControllers.AddUnique(AlivePlayerControllers[DeadPlayerIndex]);
	AlivePlayerControllers.RemoveAt(DeadPlayerIndex);
}

void AFCGameMode::PlayerAlive(APlayerController* DeadPlayer)
{
	AFCPlayerController* PC = Cast<AFCPlayerController>(DeadPlayer);
	if (!IsValid(PC)) return;

	const int32 DeadPlayerIndex = DeadPlayerControllers.Find(PC);
	if (DeadPlayerIndex == INDEX_NONE) return;
	
	AlivePlayerControllers.AddUnique(DeadPlayerControllers[DeadPlayerIndex]);
	DeadPlayerControllers.RemoveAt(DeadPlayerIndex);
}

void AFCGameMode::OnMainTimerElapsed()
{
	AFCGameState* FCGS = GetGameState<AFCGameState>();
	if (!IsValid(FCGS))
	{
		return;
	}
	
	switch (FCGS->MatchState)
	{
	case EMatchState::None:
		break;
		
	case EMatchState::Waiting:
		
		if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			bReadyForPlay = false;
		}
		else
		{
			bAllPlayersReady = true;
			
			for (APlayerState* PS : FCGS->PlayerArray)
			{
				AFCPlayerState* FCPS = Cast<AFCPlayerState>(PS);
				
				if (IsValid(FCPS))
				{
					if (!FCPS->bIsReady)
					{
						bAllPlayersReady = false;
						RemainTimeForPlaying = WaitingTime;
						break;
					}
				}
			}
		}
		
		if (bAllPlayersReady)
		{
			bReadyForPlay = true;
			--RemainTimeForPlaying;
			UE_LOG(LogTemp, Warning, TEXT("Waiting 남은 시간 : %d"), RemainTimeForPlaying);
		}
		
		// WaitingRemainTime 업데이트 (0일 때도 표시하기 위해 감소 후 업데이트)
		FCGS->SetRemainWaitingTime(RemainTimeForPlaying);
		
		// RemainTimeForPlaying이 0 이하가 되면 다음 틱에서 전환
		// 0일 때는 한 번 더 Waiting 상태로 0을 표시하고, 다음 틱에서 Playing으로 전환
		if (RemainTimeForPlaying < 0)
		{
			FCGS->MatchState = EMatchState::Playing;
			FCGS->SetRemainGameTime(GameTimeLimit);
			
			// 플레이어들을 순간이동 위치로 이동
			if (PlayerTeleportLocation != FVector::ZeroVector)
			{
				TeleportAllPlayersToLocation(PlayerTeleportLocation, PlayerTeleportRotation);
			}
			
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				AFCPlayerController* FCPC = Cast<AFCPlayerController>(*It);
				
				if (IsValid(FCPC))
				{
					FCPC->ClientRPCShowTimerWidget();
				}
			}
		}
		break;
		
	case EMatchState::Playing:
	{
		int32 CurrentRemainTime = FCGS->GetRemainGameTime();
		--CurrentRemainTime;
		FCGS->SetRemainGameTime(CurrentRemainTime);
		UE_LOG(LogTemp, Warning, TEXT("Playing 남은 시간 : %d"), CurrentRemainTime);
		
		if (AlivePlayerControllers.Num() <= 0 || CurrentRemainTime <= 0)
		{
			FCGS->MatchState = EMatchState::Ending;
			
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				AFCPlayerController* FCPC = Cast<AFCPlayerController>(*It);
				
				if (IsValid(FCPC))
				{
					FCPC->ClientRPCRemoveTimerWidget();
					
					FCPC->ClientRPCSetInputUIOnly();
					
					FCPC->ClientRPCShowResultWidget(false);
				}
			}
		}
		break;
	}
		
	case EMatchState::Ending:
		
		--RemainEndingTime;
		UE_LOG(LogTemp, Warning, TEXT("Ending 남은 시간 : %d"), RemainEndingTime);
		
		if (RemainEndingTime <= 0)
		{
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				AFCPlayerController* FCPC = Cast<AFCPlayerController>(*It);
				
				if (IsValid(FCPC))
				{
					FCPC->ClientRPCRemoveResultWidget();
				}
			}
			
			GetWorld()->ServerTravel(LobbyMapPath + TEXT("?listen"));
		}
		break;
	
	case EMatchState::End:
		break;
	
	default:
		break;
	}
}

void AFCGameMode::ResetValues()
{
	// AFCGameState* FCGS = GetGameState<AFCGameState>();
	//
	// for (APlayerState* PS : FCGS->PlayerArray)
	// {
	// 	AFCPlayerState* FCPS = Cast<AFCPlayerState>(PS);
	// 	if (IsValid(FCPS))
	// 	{
	// 		FCPS->bIsReady = false;
	// 	}
	// }
	
	GetWorldTimerManager().ClearTimer(MainTimerHandle);
	
	bReadyForPlay = false;
	bAllPlayersReady = false;
	RemainTimeForPlaying = WaitingTime;
	
	AFCGameState* FCGS = GetGameState<AFCGameState>();
	if (IsValid(FCGS))
	{
		FCGS->SetRemainGameTime(GameTimeLimit);
		FCGS->SetRemainWaitingTime(WaitingTime);
	}
	
	RemainEndingTime = EndingTimeLimit;
}

USpawnManager* AFCGameMode::GetSpawnManger()
{
	if (!HasAuthority()) return nullptr;

	if (!IsValid(SpawnManager))
	{
		SpawnManager = NewObject<USpawnManager>(this);
		if (IsValid(ItemSpawnData))
		{
			GameRequireKey = SpawnManager->Initialize(ItemSpawnData);
		}
	}

	return SpawnManager;
}

void AFCGameMode::StartSpawn()
{
	SpawnManager->SpawnAllItems();
}

void AFCGameMode::TeleportAllPlayersToLocation(const FVector& Location, const FRotator& Rotation)
{
	if (!HasAuthority())
	{
		return;
	}

	// AlivePlayerControllers를 우선 사용, 없으면 모든 플레이어 컨트롤러 사용
	TArray<APlayerController*> ControllersToTeleport;
	
	if (AlivePlayerControllers.Num() > 0)
	{
		for (APlayerController* PC : AlivePlayerControllers)
		{
			if (IsValid(PC))
			{
				ControllersToTeleport.Add(PC);
			}
		}
	}
	else
	{
		// AlivePlayerControllers가 비어있으면 모든 플레이어 컨트롤러 사용
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (IsValid(PC))
			{
				ControllersToTeleport.Add(PC);
			}
		}
	}

	// 모든 플레이어 컨트롤러를 순회하며 순간이동
	for (APlayerController* PC : ControllersToTeleport)
	{
		if (!IsValid(PC))
		{
			continue;
		}

		APawn* Pawn = PC->GetPawn();
		if (!IsValid(Pawn))
		{
			continue;
		}

		// Character인 경우 MovementComponent의 속도 초기화
		if (ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
			{
				MovementComp->Velocity = FVector::ZeroVector;
			}
		}

		// 위치와 회전 설정
		Pawn->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
		Pawn->SetActorRotation(Rotation);
	}
}
