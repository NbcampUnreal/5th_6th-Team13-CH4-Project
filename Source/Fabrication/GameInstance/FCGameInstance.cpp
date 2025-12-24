#include "GameInstance/FCGameInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Engine/LocalPlayer.h"

UFCGameInstance::UFCGameInstance()
{
	// 생성자에서 초기화
}

void UFCGameInstance::Init()
{
	Super::Init();
	
	// GameInstance 초기화 시 세션 인터페이스 가져오기
	InitSessionInterface();
}

void UFCGameInstance::InitSessionInterface()
{
	// OnlineSubsystem을 통해 세션 인터페이스 가져오기
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("세션 인터페이스를 성공적으로 가져왔습니다."));
			
			// 세션 생성 완료 콜백 델리게이트 바인딩
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
				this, 
				&UFCGameInstance::OnCreateSessionComplete
			);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("세션 인터페이스를 가져올 수 없습니다."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem을 찾을 수 없습니다."));
	}
}

void UFCGameInstance::SetLocalPlayerNickName(const FString& NewName)
{
	LocalPlayerNickName = NewName;
	UE_LOG(LogTemp, Error, TEXT("%s"), *GetLocalPlayerNickName())
}

const FString& UFCGameInstance::GetLocalPlayerNickName() const
{
	return LocalPlayerNickName;
}

void UFCGameInstance::CreateSession(const FString& SessionName, int32 MaxPlayers)
{
	// 세션 인터페이스가 유효한지 확인
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("세션 인터페이스가 유효하지 않습니다."));
		return;
	}
	
	// 이미 존재하는 세션이 있다면 제거
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("기존 세션이 존재합니다. 제거 후 새로 생성합니다."));
		SessionInterface->DestroySession(NAME_GameSession);
	}
	
	// 세션 설정 생성
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	
	// 세션 설정 값들
	SessionSettings->bIsLANMatch = false; // 온라인 매치 (true면 LAN)
	SessionSettings->NumPublicConnections = MaxPlayers; // 최대 플레이어 수
	SessionSettings->bAllowInvites = true; // 초대 허용
	SessionSettings->bAllowJoinInProgress = false; // 게임 중 참가 허용
	SessionSettings->bShouldAdvertise = true; // 다른 플레이어에게 공개
	SessionSettings->bUsesPresence = true; // 프레즌스 사용 (Steam 등에서 필요)
	SessionSettings->bUseLobbiesIfAvailable = true; // 로비 사용 가능하면 사용
	
	// 세션 이름을 설정에 추가
	SessionSettings->Set(FName(TEXT("SESSION_NAME")), SessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// 로컬 플레이어 가져오기
	ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("로컬 플레이어를 찾을 수 없습니다."));
		return;
	}
	
	// 세션 생성 요청
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings))
	{
		UE_LOG(LogTemp, Error, TEXT("세션 생성 요청에 실패했습니다."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 생성 요청을 보냈습니다: %s (최대 %d명)"), *SessionName, MaxPlayers);
	}
}

void UFCGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 생성 성공: %s"), *SessionName.ToString());
		// 여기에 성공 시 처리할 로직 추가 (예: 로비로 이동)
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("세션 생성 실패: %s"), *SessionName.ToString());
	}
}
