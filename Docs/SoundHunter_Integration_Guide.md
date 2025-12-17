# SoundHunter 연동 가이드

> **작성일**: 2024-12-16
> **대상**: Player 팀, Item 팀
> **작성자**: Monster 팀 (Team C)

---

## 1. 개요

SoundHunter는 **소리를 감지하여 플레이어를 추적**하는 몬스터입니다.

### 핵심 특징
- 시야(Sight)보다 청각(Hearing)에 더 민감
- **Lure(유인) 아이템**에 무조건 반응 (최우선)
- 일반 소리는 이미 타겟이 있으면 무시 (집중 모드)

---

## 2. 소리 발생 방법

### 2.1 기본 함수: `MakeNoise()`

모든 Actor에서 호출 가능한 함수입니다.

```cpp
// 함수 시그니처
void MakeNoise(
    float Loudness,           // 볼륨 (0.0 ~ 1.0)
    APawn* NoiseInstigator,   // 소리를 낸 주체 (nullptr 가능)
    FVector NoiseLocation,    // 소리 발생 위치
    FName Tag = NAME_None     // 소리 태그 ("Lure" 등)
);
```

### 2.2 파라미터 설명

| 파라미터 | 타입 | 설명 |
|----------|------|------|
| `Loudness` | float | 0.0~1.0 범위. 실제 감지 거리 = HearingRange × Loudness |
| `NoiseInstigator` | APawn* | 소리를 낸 주체. 플레이어면 해당 플레이어가 타겟이 됨 |
| `NoiseLocation` | FVector | 소리가 발생한 위치. 몬스터가 이 위치로 이동 |
| `Tag` | FName | `"Lure"` 태그 시 유인 아이템으로 인식 |

---

## 3. 소리 태그 시스템

### 3.1 일반 소리 (태그 없음)

```cpp
// 플레이어가 달리기
MakeNoise(0.5f, this, GetActorLocation());

// 문이 닫히는 소리
MakeNoise(0.4f, PlayerWhoClosedDoor, DoorLocation);
```

**동작:**
- SoundHunter가 **타겟이 없을 때만** 반응
- 이미 다른 플레이어를 추적 중이면 무시됨
- 플레이어 간 소리로 서로 어그로 뺏기 불가

### 3.2 Lure 소리 (태그: `"Lure"`)

```cpp
// 알람시계 아이템
MakeNoise(1.0f, nullptr, GetActorLocation(), FName("Lure"));

// 소리나는 인형
MakeNoise(0.8f, nullptr, GetActorLocation(), FName("Lure"));
```

**동작:**
- SoundHunter가 **무조건 반응** (최우선)
- 기존 추적 타겟을 버리고 Lure 위치로 이동
- 유인 아이템의 핵심 기능

---

## 4. 구현 예시

### 4.1 플레이어 이동 소리 (Player 팀)

플레이어 이동 소리를 구현하는 방법은 3가지가 있습니다. 프로젝트 상황에 맞게 선택하세요.

#### 방법 A: Tick에서 속도 체크 (가장 간단, 빠른 테스트용)

```cpp
// FCPlayerCharacter.h에 추가
protected:
    float FootstepNoiseTimer = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    float FootstepNoiseInterval = 0.3f;  // 소리 발생 간격

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    float RunningSpeedThreshold = 400.0f;  // 이 속도 이상이면 달리기
```

```cpp
// FCPlayerCharacter.cpp - Tick() 함수 내부에 추가
void AFCPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ... 기존 AimPitch 코드 ...

    // === 이동 소리 발생 (서버에서만) ===
    if (HasAuthority())
    {
        const float Speed = GetVelocity().Size();

        if (Speed > RunningSpeedThreshold)
        {
            // 달리기 중 - 주기적으로 소리 발생
            FootstepNoiseTimer += DeltaTime;
            if (FootstepNoiseTimer >= FootstepNoiseInterval)
            {
                MakeNoise(0.5f, this, GetActorLocation());
                FootstepNoiseTimer = 0.0f;
            }
        }
        else if (Speed > 10.0f)
        {
            // 걷기 중 - 더 긴 간격으로 작은 소리
            FootstepNoiseTimer += DeltaTime;
            if (FootstepNoiseTimer >= FootstepNoiseInterval * 2.0f)
            {
                MakeNoise(0.1f, this, GetActorLocation());
                FootstepNoiseTimer = 0.0f;
            }
        }
        else
        {
            // 정지 시 타이머 리셋
            FootstepNoiseTimer = 0.0f;
        }
    }
}
```

**장점:** 가장 간단, 빠르게 테스트 가능
**단점:** Tick에서 매 프레임 체크

---

#### 방법 B: Animation Notify (Blend Space 미사용 시 권장)

> **주의:** Blend Space를 사용하는 경우 이 방법은 권장하지 않습니다.
> Blend Space에서는 여러 애니메이션이 블렌딩되면서 Notify가 중복 호출되거나 타이밍이 불규칙해질 수 있습니다.
> Blend Space 사용 시 **방법 A (Tick)** 또는 **방법 C (Component)**를 사용하세요.

발소리 애니메이션에 Notify를 추가하여 정확한 타이밍에 소리 발생.

**1단계: AnimNotify 클래스 생성**

```cpp
// AnimNotify_Footstep.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Footstep.generated.h"

UCLASS()
class FABRICATION_API UAnimNotify_Footstep : public UAnimNotify
{
    GENERATED_BODY()

public:
    // 달리기 발소리인지 여부 (애니메이션별로 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
    bool bIsRunning = false;

    // 볼륨 직접 지정 (0이면 bIsRunning에 따라 자동 결정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
    float CustomLoudness = 0.0f;

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
```

```cpp
// AnimNotify_Footstep.cpp
#include "AnimNotify_Footstep.h"
#include "GameFramework/Pawn.h"

void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner || !Owner->HasAuthority()) return;

    // 볼륨 결정
    float Loudness = CustomLoudness;
    if (FMath::IsNearlyZero(Loudness))
    {
        Loudness = bIsRunning ? 0.5f : 0.1f;
    }

    // 소리 발생
    Owner->MakeNoise(Loudness, Cast<APawn>(Owner), Owner->GetActorLocation());
}
```

**2단계: 애니메이션에 Notify 추가**
- 언리얼 에디터에서 걷기/달리기 애니메이션 열기
- 발이 땅에 닿는 프레임에 `AnimNotify_Footstep` 추가
- 달리기 애니메이션에는 `bIsRunning = true` 설정

**장점:** 발소리 타이밍이 애니메이션과 정확히 일치
**단점:** 애니메이션 에디터 작업 필요

---

#### 방법 C: SpeedControlComponent 활용 (기존 컴포넌트 활용)

이미 존재하는 `SpeedControlComponent`에서 타이머 기반으로 처리.

```cpp
// SpeedControlComponent.h에 추가
protected:
    FTimerHandle FootstepNoiseTimerHandle;

    void StartFootstepNoise(float Loudness, float Interval);
    void StopFootstepNoise();
    void EmitFootstepNoise();

    float CurrentFootstepLoudness = 0.0f;
```

```cpp
// SpeedControlComponent.cpp에 추가
void USpeedControlComponent::StartFootstepNoise(float Loudness, float Interval)
{
    if (!GetOwner()->HasAuthority()) return;

    CurrentFootstepLoudness = Loudness;

    GetWorld()->GetTimerManager().SetTimer(
        FootstepNoiseTimerHandle,
        this,
        &USpeedControlComponent::EmitFootstepNoise,
        Interval,
        true  // 반복
    );
}

void USpeedControlComponent::StopFootstepNoise()
{
    GetWorld()->GetTimerManager().ClearTimer(FootstepNoiseTimerHandle);
}

void USpeedControlComponent::EmitFootstepNoise()
{
    if (APawn* Owner = Cast<APawn>(GetOwner()))
    {
        Owner->MakeNoise(CurrentFootstepLoudness, Owner, Owner->GetActorLocation());
    }
}

// 속도 변경 시 호출 (기존 함수에서 호출)
void USpeedControlComponent::OnSpeedChanged(bool bIsRunning)
{
    if (bIsRunning)
    {
        StartFootstepNoise(0.5f, 0.3f);  // 달리기: 큰 소리, 짧은 간격
    }
    else
    {
        StopFootstepNoise();  // 걷기는 소리 안 냄 (또는 작은 소리)
    }
}
```

**장점:** 기존 컴포넌트 활용, 속도 상태와 연동
**단점:** SpeedControlComponent 수정 필요

---

#### 방법 비교

| 방법 | 난이도 | 자연스러움 | 성능 | 권장 상황 |
|------|--------|-----------|------|----------|
| A. Tick | 쉬움 | 보통 | 보통 | 빠른 테스트 |
| B. Anim Notify | 중간 | 최고 | 좋음 | 최종 구현 |
| C. Component | 쉬움 | 보통 | 좋음 | 기존 구조 활용 |

**추천:** 테스트는 방법 A로 빠르게, 나중에 방법 B로 다듬기

### 4.2 유인 아이템 (Item 팀)

```cpp
// AlarmClockItem.cpp

void AAlarmClockItem::Activate()
{
    // 서버에서만 실행
    if (!HasAuthority()) return;

    bIsActive = true;

    // 알람 시작 - Lure 태그로 SoundHunter 유인
    GetWorld()->GetTimerManager().SetTimer(
        AlarmTimerHandle,
        this,
        &AAlarmClockItem::PlayAlarmSound,
        AlarmInterval,  // 예: 1.0초 간격
        true            // 반복
    );
}

void AAlarmClockItem::PlayAlarmSound()
{
    // Lure 태그 필수!
    MakeNoise(1.0f, nullptr, GetActorLocation(), FName("Lure"));

    // 사운드 이펙트 재생 (클라이언트용)
    Multicast_PlayAlarmSFX();

    // 일정 시간 후 자동 종료
    AlarmPlayCount++;
    if (AlarmPlayCount >= MaxAlarmCount)
    {
        Deactivate();
    }
}

void AAlarmClockItem::Deactivate()
{
    bIsActive = false;
    GetWorld()->GetTimerManager().ClearTimer(AlarmTimerHandle);
}
```

### 4.3 환경 오브젝트 소리 (Objects 팀)

```cpp
// InteractableDoor.cpp

void AInteractableDoor::CloseDoor(ACharacter* Closer)
{
    // 문 닫기 애니메이션...

    // 서버에서 소리 발생
    if (HasAuthority())
    {
        // 문 닫은 플레이어가 Instigator
        // 일반 소리이므로 SoundHunter가 이미 타겟 있으면 무시됨
        MakeNoise(0.4f, Cast<APawn>(Closer), GetActorLocation());
    }
}

void AInteractableDoor::SlamDoor(ACharacter* Slammer)
{
    // 문 세게 닫기 - 더 큰 소리
    if (HasAuthority())
    {
        MakeNoise(0.7f, Cast<APawn>(Slammer), GetActorLocation());
    }
}
```

---

## 5. Loudness 가이드라인

### 5.1 플레이어 행동

| 행동 | Loudness | 감지 거리* | 비고 |
|------|----------|-----------|------|
| 걷기 | 0.0 | 0 | 소리 없음 |
| 살금살금 걷기 | 0.05 | 150 | 거의 안 들림 |
| 일반 걷기 | 0.1 | 300 | 가까이 가야 들림 |
| 달리기 | 0.4~0.5 | 1200~1500 | 위험 |
| 점프 착지 | 0.2~0.4 | 600~1200 | 높이에 따라 |
| 웅크리기 이동 | 0.02 | 60 | 거의 무음 |

### 5.2 아이템/환경

| 소리 | Loudness | 태그 | 감지 거리* |
|------|----------|------|-----------|
| 문 닫기 | 0.3~0.4 | 없음 | 900~1200 |
| 문 세게 닫기 | 0.6~0.7 | 없음 | 1800~2100 |
| 유리 깨짐 | 0.5 | 없음 | 1500 |
| 알람시계 | 1.0 | `"Lure"` | 3000 (최대) |
| 소리 인형 | 0.7~0.8 | `"Lure"` | 2100~2400 |
| 폭죽 | 1.0 | `"Lure"` | 3000 (최대) |

> *감지 거리 = SoundHunter의 HearingRange(기본 3000) × Loudness

---

## 6. 주의사항

### 6.1 서버에서만 호출

```cpp
// 올바른 예
if (HasAuthority())
{
    MakeNoise(0.5f, this, GetActorLocation());
}

// 잘못된 예 - 클라이언트에서도 호출됨
MakeNoise(0.5f, this, GetActorLocation());  // 중복 처리 위험!
```

### 6.2 Instigator 설정

```cpp
// 플레이어가 소리를 냈을 때 - 플레이어를 Instigator로
MakeNoise(0.5f, PlayerCharacter, Location);
// → SoundHunter가 해당 플레이어를 타겟으로 설정

// 아이템이 소리를 냈을 때 - nullptr 또는 아이템 자체
MakeNoise(1.0f, nullptr, ItemLocation, FName("Lure"));
// → SoundHunter가 위치만 추적 (타겟 플레이어 없음)
```

### 6.3 Lure 태그 철자 주의

```cpp
// 올바른 예
FName("Lure")

// 잘못된 예 - 대소문자 다름!
FName("lure")   // 작동 안 함
FName("LURE")   // 작동 안 함
```

---

## 7. 테스트 방법

### 7.1 로그 확인

SoundHunter는 소리 감지 시 로그를 출력합니다:

```
[SoundHunter] Hearing - LURE detected at: X=1000 Y=2000 Z=0
[SoundHunter] Hearing - New target Player at: X=500 Y=300 Z=0
[SoundHunter] Hearing - Ignored (already has target)
```

### 7.2 디버그 시각화 (에디터)

1. 에디터에서 `'` 키로 AI 디버깅 활성화
2. Perception 범위가 초록색 원으로 표시됨
3. 감지된 소리 위치에 마커 표시

---

## 8. FAQ

### Q: 벽 뒤에서 소리를 내면 들리나요?
**A:** 네, 현재 벽을 무시하고 직선 거리로만 계산합니다. Lure 아이템이 확실히 작동하도록 의도된 설계입니다.

### Q: 여러 플레이어가 동시에 소리를 내면?
**A:** SoundHunter가 이미 타겟이 있으면 다른 일반 소리는 무시합니다. 단, Lure 소리는 무조건 반응합니다.

### Q: Lure 아이템으로 유인 후 어떻게 되나요?
**A:** SoundHunter가 Lure 위치에 도착하거나 일정 시간(기본 10초) 후 Lure 상태가 해제되고 다시 소리를 듣기 시작합니다.

### Q: 소리 발생 빈도는?
**A:** 너무 자주 호출하면 서버 부하가 생길 수 있습니다. 달리기의 경우 0.3~0.5초 간격을 권장합니다.

---

## 9. 연락처

구현 관련 문의: **Monster 팀 (Team C)**

- SoundHunter 클래스: `Source/Fabrication/Monster/FCSoundHunter.h`
- AI Controller: `Source/Fabrication/MonsterController/FCSoundHunterAIController.h`