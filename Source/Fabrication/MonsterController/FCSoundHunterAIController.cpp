// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterController/FCSoundHunterAIController.h"
#include "MonsterController/FCMonsterBlackboardKeys.h"
#include "Monster/FCSoundHunter.h"
#include "Player/FCPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Fabrication.h"
#include "DrawDebugHelpers.h"

AFCSoundHunterAIController::AFCSoundHunterAIController()
{
	// 청력(Hearing) 설정 생성 (실제 값은 OnPossess에서 ApplyHearingConfig를 통해 적용)
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;   // 아이템(Lure) 감지
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
}

AFCSoundHunter* AFCSoundHunterAIController::GetSoundHunter() const
{
	return Cast<AFCSoundHunter>(GetPawn());
}

void AFCSoundHunterAIController::OnPossess(APawn* InPawn)
{
	// 부모 OnPossess 호출 (BehaviorTree 실행 등)
	Super::OnPossess(InPawn);

	// [멀티플레이] 서버에서만 실행
	if (!HasAuthority()) return;

	// Hearing 설정 적용 (블루프린트에서 설정한 값으로)
	ApplyHearingConfig();

	// [버그 수정] 부모에서 바인딩한 델리게이트 제거 후 SoundHunter 전용으로 교체
	// 부모의 OnTargetPerceptionUpdated는 Hearing을 처리하지 않고 SeenPlayer를 잘못 설정함
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.Clear();
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
			this, &AFCSoundHunterAIController::OnPerceptionUpdated_SoundHunter);
	}
}

void AFCSoundHunterAIController::OnPerceptionUpdated_SoundHunter(AActor* Actor, FAIStimulus Stimulus)
{
	// [멀티플레이] 서버 전용
	if (!HasAuthority()) return;

	// Hearing 자극 처리
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		HandleHearingStimulus(Actor, Stimulus);
		return;  // Hearing은 여기서 처리 완료
	}

	// Sight 자극 처리 (부모 로직 재사용)
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		HandlePerceptionUpdated(Actor, Stimulus);
	}
}

void AFCSoundHunterAIController::HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 부모의 공통 Sight 처리 함수 호출
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);
	if (!Player) return;

	HandleSightStimulus(Player, Stimulus);
}

void AFCSoundHunterAIController::HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
	AFCSoundHunter* Monster = GetSoundHunter();
	if (!Monster) return;

	// 소리 감지 실패 시 무시
	if (!Stimulus.WasSuccessfullySensed()) return;

	// [Vanish/Attack 상태 체크] Vanish 상태이거나 공격 중이면 모든 소리 무시
	// - bIsVanished: 공격 후 사라진 상태 (Respawn 완료 후 반응)
	// - !bCanAttack: 공격 모션 진행 중 (Attack→Vanish 시퀀스 보호)
	if (Monster->bIsVanished || !Monster->bCanAttack)
	{
		return;
	}

	const FVector SoundLocation = Stimulus.StimulusLocation;
	const float SoundLoudness = Stimulus.Strength;

	// "Lure" 태그 체크 (유인 아이템) - 무조건 최우선 반응!
	if (Stimulus.Tag == FName("Lure"))
	{
		// Lure는 최우선! 현재 타겟 무시하고 Lure 위치로
		Monster->SetLureTarget(SoundLocation);

		// 기존 타겟 초기화 (Lure에 집중)
		Monster->TargetPlayer = nullptr;

		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool(FCMonsterBBKeys::bHasLureTarget, true);
			BlackboardComp->SetValueAsVector(FCMonsterBBKeys::LureLocation, SoundLocation);
			BlackboardComp->ClearValue(FCMonsterBBKeys::TargetPlayer);
		}

		FC_LOG_NET(LogFCNet, Log, TEXT("Hearing - LURE detected! Location: %s, Loudness: %.2f"), *SoundLocation.ToString(), SoundLoudness);

#if WITH_EDITOR
		// 디버그 시각화: 초록색 구체 (Lure)
		DrawDebugSphere(GetWorld(), SoundLocation, 80.0f, 16, FColor::Green, false, 3.0f, 0, 2.0f);
		DrawDebugString(GetWorld(), SoundLocation + FVector(0, 0, 100), TEXT("LURE!"), nullptr, FColor::Green, 3.0f);
#endif
		return;
	}

	// === 일반 소리: 이미 타겟이 있거나 Lure 추적 중이면 무시 ===
	if (Monster->TargetPlayer != nullptr || Monster->bHasLureTarget)
	{
		FC_LOG_NET(LogFCNet, Log, TEXT("Hearing - Ignored (already has target). Sound at: %s"), *SoundLocation.ToString());
		return;
	}

	// === Investigate 중일 때: 새 소리가 더 가까울 때만 반응 ===
	if (Monster->bHasHeardSound)
	{
		const float CurrentDist = FVector::Dist(Monster->GetActorLocation(), Monster->LastHeardLocation);
		const float NewDist = FVector::Dist(Monster->GetActorLocation(), SoundLocation);

		if (NewDist >= CurrentDist)
		{
			FC_LOG_NET(LogFCNet, Log, TEXT("Hearing - Ignored (new sound is farther). Current: %.0f, New: %.0f"),
				CurrentDist, NewDist);
			return;
		}

		FC_LOG_NET(LogFCNet, Log, TEXT("Hearing - New sound is closer! Updating. Current: %.0f, New: %.0f"),
			CurrentDist, NewDist);
	}

	// 타겟이 없을 때만 새 소리에 반응 → Investigate (소리 위치로 이동)
	// [수정] 플레이어 소리도 Chase가 아닌 Investigate로 처리 (시야로 봐야만 Chase)
	Monster->SetHeardSound(SoundLocation);
	Monster->LastHeardLocation = SoundLocation;

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(FCMonsterBBKeys::bHasHeardSound, true);
		BlackboardComp->SetValueAsVector(FCMonsterBBKeys::LastHeardLocation, SoundLocation);
	}

	// 플레이어 소리인지 확인 (디버그용)
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(Actor);

	if (Player)
	{
		FC_LOG_NET(LogFCNet, Log, TEXT("Hearing - Player footstep → Investigate! Player: %s, Location: %s, Loudness: %.2f"),
			*Player->GetName(), *SoundLocation.ToString(), SoundLoudness);

#if WITH_EDITOR
		// 디버그 시각화: 주황색 구체 (플레이어 소리 → Investigate)
		DrawDebugSphere(GetWorld(), SoundLocation, 50.0f, 12, FColor::Orange, false, 3.0f, 0, 2.0f);
		DrawDebugString(GetWorld(), SoundLocation + FVector(0, 0, 80), FString::Printf(TEXT("Footstep→Investigate (%.1f)"), SoundLoudness), nullptr, FColor::Orange, 3.0f);
#endif
	}
	else
	{
		FC_LOG_NET(LogFCNet, Log, TEXT("Hearing - General sound → Investigate! Actor: %s, Location: %s, Loudness: %.2f"),
			Actor ? *Actor->GetName() : TEXT("None"), *SoundLocation.ToString(), SoundLoudness);

#if WITH_EDITOR
		// 디버그 시각화: 노란색 구체 (기타 소리)
		DrawDebugSphere(GetWorld(), SoundLocation, 60.0f, 12, FColor::Yellow, false, 3.0f, 0, 2.0f);
		DrawDebugString(GetWorld(), SoundLocation + FVector(0, 0, 100),
			FString::Printf(TEXT("Sound (%.1f)"), SoundLoudness), nullptr, FColor::Yellow, 3.0f);
#endif
	}
}

void AFCSoundHunterAIController::ApplyHearingConfig()
{
	if (!HearingConfig || !AIPerceptionComponent) return;

	// Monster의 DataTable 데이터 확인
	AFCSoundHunter* Monster = GetSoundHunter();
	if (Monster && Monster->bDataTableLoaded)
	{
		// DataTable에서 로드된 값 사용
		const FFCMonsterDataRow& Data = Monster->CachedMonsterData;
		HearingRadius = Data.HearingRadius;
		bHearingEnabled = Data.bHearingEnabled;

		FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Hearing 값을 DataTable에서 로드"), *GetName());
	}

	// Hearing 설정 적용
	HearingConfig->HearingRange = HearingRadius;

	// Perception 컴포넌트에 설정 적용
	AIPerceptionComponent->ConfigureSense(*HearingConfig);

	// 청력이 활성화된 경우에만 Dominant Sense로 설정 (SoundHunter는 청력이 더 중요)
	if (bHearingEnabled)
	{
		AIPerceptionComponent->SetDominantSense(HearingConfig->GetSenseImplementation());
	}

	FC_LOG_NET(LogFCNet, Log, TEXT("[%s] Hearing Config 적용 - Radius: %.0f, Enabled: %s"),
		*GetName(), HearingRadius, bHearingEnabled ? TEXT("true") : TEXT("false"));
}