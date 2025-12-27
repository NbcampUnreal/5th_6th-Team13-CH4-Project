#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItemBase.h"
#include "NoiseItem.generated.h"

class UAIPerceptionStimuliSourceComponent;
class USoundCue;
class USoundAttenuation;

UCLASS()
class FABRICATION_API ANoiseItem : public APickupItemBase
{
	GENERATED_BODY()
	
public:
	ANoiseItem();

	virtual void BeginPlay() override; 
	//Noise Setting 
	UFUNCTION()
	void ActivateNoise();
	//Make Noise  
	UFUNCTION()
	void TriggerNoise(); //AI Perception(청각 센스)에 “소리 이벤트 발생"을 알려줌 

#pragma region RPC
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayNoiseSound(FVector Loc);//모든 클라이언트가 듣는 소리신호 
#pragma endregion

#pragma region Var 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseItem", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Loudness = 1.0f; //소리 크기 0.0 ~1.0f 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseItem")
	float ActivationDelay = 10.0f; //소리 발생 시작까지 딜레이 (초)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseItem")
	bool bRepeatNoise = true; //반복 발생 여부 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseItem", meta = (EditCondition = "bRepeatNoise"))
	float RepeatInterval = 3.0f; //반복 간격(초)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseItem")
	float Duration = 30.0f; //지속 시간(초, 0==무제한)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseItem")
	bool bUseLureTag = true; //Lure 태그 사용 여부 (true == 최우선 반응)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NoiseItem")
	TObjectPtr<USoundCue> NoiseSound; //실제 소리 

	//멀이질 수록 체감상 소리가 작아지게 Attenuation 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseItem")
	TObjectPtr<USoundAttenuation> NoiseSoundAttenuation;  

	//AI Perception(Sight/Hearing)에 NoiseItem 감지 대상 등록 + Hearing Sense에 반응 설정 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource; 

private:
	FTimerHandle NoiseTimerHandle; //실제 소리 타이머 
	FTimerHandle DurationTimerHandle;//NoiseItem 수명 
#pragma endregion 
};
