#include "GameMode/FCGameMode_Loading.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Controller/FCPlayerController_Loading.h"

AFCGameMode_Loading::AFCGameMode_Loading()
	: LoadingDelay(2.0f)
	, CurrentProgress(0.0f)
{
	// 로딩 맵 전용 PlayerController 설정
	PlayerControllerClass = AFCPlayerController_Loading::StaticClass();
}

void AFCGameMode_Loading::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// URL에서 NextMap 파라미터 읽기
		ParseNextMapFromURL();

		// NextMap이 없으면 기본 게임 맵 경로 사용
		if (NextMapPath.IsEmpty())
		{
			NextMapPath = TEXT("/Game/Fabrication/Maps/TempMap");
			UE_LOG(LogTemp, Warning, TEXT("로딩 맵: NextMap 파라미터가 없어 기본 게임 맵을 사용합니다: %s"), *NextMapPath);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("로딩 맵: 다음 맵 경로를 읽었습니다: %s"), *NextMapPath);
		}

		// 초기 진행률 전달
		CurrentProgress = 0.0f;
		BroadcastLoadingProgress(CurrentProgress);

		// 진행률 업데이트 타이머 시작 (0.1초마다)
		GetWorldTimerManager().SetTimer(
			ProgressUpdateTimerHandle,
			this,
			&AFCGameMode_Loading::UpdateLoadingProgress,
			0.1f,
			true
		);

		// 일정 시간 후 게임 맵으로 이동
		GetWorldTimerManager().SetTimer(
			TravelTimerHandle,
			this,
			&AFCGameMode_Loading::TravelToGameMap,
			LoadingDelay,
			false
		);
	}
}

void AFCGameMode_Loading::ParseNextMapFromURL()
{
	if (!HasAuthority()) return;

	// 현재 World의 URL 가져오기
	FString CurrentURL = GetWorld()->GetAddressURL();
	
	// URL에서 NextMap 파라미터 찾기
	FString NextMapParam = TEXT("NextMap=");
	int32 NextMapIndex = CurrentURL.Find(NextMapParam);
	
	if (NextMapIndex != INDEX_NONE)
	{
		// NextMap= 이후의 문자열 추출
		int32 StartIndex = NextMapIndex + NextMapParam.Len();
		FString RemainingURL = CurrentURL.Mid(StartIndex);
		
		// 다음 파라미터나 끝까지 추출
		int32 EndIndex = RemainingURL.Find(TEXT("?"));
		if (EndIndex != INDEX_NONE)
		{
			NextMapPath = RemainingURL.Left(EndIndex);
		}
		else
		{
			NextMapPath = RemainingURL;
		}
	}
}

void AFCGameMode_Loading::UpdateLoadingProgress()
{
	if (!HasAuthority()) return;

	// 시간에 따라 진행률 증가 (0.0 ~ 1.0)
	float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(TravelTimerHandle);
	CurrentProgress = FMath::Clamp(ElapsedTime / LoadingDelay, 0.0f, 1.0f);

	// 모든 플레이어에게 진행률 전달
	BroadcastLoadingProgress(CurrentProgress);

	// 100% 도달 시 타이머 정리
	if (CurrentProgress >= 1.0f)
	{
		GetWorldTimerManager().ClearTimer(ProgressUpdateTimerHandle);
	}
}

void AFCGameMode_Loading::BroadcastLoadingProgress(float Progress)
{
	if (!HasAuthority()) return;

	// 모든 PlayerController에게 진행률 전달
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AFCPlayerController_Loading* PC = Cast<AFCPlayerController_Loading>(It->Get()))
		{
			// Client RPC로 진행률 전달
			PC->ClientRPCShowLoadingProgress(Progress);
		}
	}
}

void AFCGameMode_Loading::TravelToGameMap()
{
	if (HasAuthority())
	{
		// 진행률 업데이트 타이머 정리
		GetWorldTimerManager().ClearTimer(ProgressUpdateTimerHandle);
		
		// 100% 진행률 전달
		BroadcastLoadingProgress(1.0f);

		UE_LOG(LogTemp, Warning, TEXT("로딩 맵에서 게임 맵으로 이동: %s"), *NextMapPath);
		GetWorld()->ServerTravel(NextMapPath + TEXT("?listen"));
	}
}

