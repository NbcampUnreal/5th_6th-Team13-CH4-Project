// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterController/FCMonsterAIController.h"
#include "FCSoundHunterAIController.generated.h"

// 전방 선언
class AFCSoundHunter;
class UAISenseConfig_Hearing;

/**
 * SoundHunter 전용 AI 컨트롤러
 * - 부모의 Sight Sense에 추가로 Hearing Sense 구성
 * - Lure(유인) 태그가 붙은 소리는 최우선으로 처리
 *
 * [블랙보드 키]
 * - bHasLureTarget (Bool): Lure 타겟 존재 여부
 * - LureLocation (Vector): Lure 위치
 * - LastHeardLocation (Vector): 마지막 소리 감지 위치
 */
UCLASS()
class FABRICATION_API AFCSoundHunterAIController : public AFCMonsterAIController
{
	GENERATED_BODY()

public:
	AFCSoundHunterAIController();

	/** SoundHunter 캐스트 헬퍼 */
	UFUNCTION(BlueprintCallable, Category = "SoundHunter")
	AFCSoundHunter* GetSoundHunter() const;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	/** Perception 업데이트 (부모 오버라이드) - Hearing 처리 추가 */
	virtual void HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** Hearing 전용 처리 */
	void HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);

protected:
	/** [청력 설정] : Hearing Config */
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	/** OnPossess에서 호출 - Hearing 설정 적용 */
	virtual void ApplyHearingConfig();

#pragma region Hearing Configuration (Blueprint Editable)

public:
	/** 청력 감지 거리 (cm) - Monster의 HearingRange보다 우선 적용 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Perception|Hearing")
	float HearingRadius = 3000.0f;

	/** 청력 활성화 여부 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Perception|Hearing")
	bool bHearingEnabled = true;

#pragma endregion

private:
	/** Perception 델리게이트 (부모의 OnTargetPerceptionUpdated 대신 사용) */
	UFUNCTION()
	void OnPerceptionUpdated_SoundHunter(AActor* Actor, FAIStimulus Stimulus);
};