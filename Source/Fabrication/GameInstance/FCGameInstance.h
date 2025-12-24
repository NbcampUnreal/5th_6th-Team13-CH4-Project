#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "FCGameInstance.generated.h"

UCLASS()
class FABRICATION_API UFCGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFCGameInstance();
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable)
	void SetLocalPlayerNickName(const FString& NewName);
	UFUNCTION(BlueprintCallable)
	const FString& GetLocalPlayerNickName() const;
	
	// 세션 생성 함수 (Blueprint에서 호출 가능)
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FString& SessionName, int32 MaxPlayers = 4);

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString LocalPlayerNickName;
	
	// 세션 매니저 관련 변수들
	IOnlineSessionPtr SessionInterface; // 세션 인터페이스 포인터
	
	// 세션 인터페이스를 초기화하는 함수
	void InitSessionInterface();
	
	// 세션 생성 완료 콜백 함수
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	
};
