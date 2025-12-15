// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#pragma region NetLogging

FABRICATION_API DECLARE_LOG_CATEGORY_EXTERN(LogFCNet, Log, All);

#define NETMODE_TCHAR ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("Client%02d"), UE::GetPlayInEditorID()) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("StandAlone") : TEXT("Server")))
#define FUNCTION_TCHAR (ANSI_TO_TCHAR(__FUNCTION__))
#define FC_LOG_NET(LogCategory, Verbosity, Format, ...) UE_LOG(LogCategory, Verbosity, TEXT("[%s] %s %s"), NETMODE_TCHAR, FUNCTION_TCHAR, *FString::Printf(Format, ##__VA_ARGS__))

#define LOCAL_ROLE_TCHAR *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define REMOTE_ROLE_TCHAR *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))
#define FC_LOG_ROLE(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), NETMODE_TCHAR, LOCAL_ROLE_TCHAR, REMOTE_ROLE_TCHAR, FUNCTION_TCHAR, *FString::Printf(Format, ##__VA_ARGS__))

#define SUBOBJECT_LOCAL_ROLE_TCHAR *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define SUBOBJECT_REMOTE_ROLE_TCHAR *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))
#define FC_LOG_SUBOBJECT_ROLE(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), NETMODE_TCHAR, SUBOBJECT_LOCAL_ROLE_TCHAR, SUBOBJECT_REMOTE_ROLE_TCHAR, FUNCTION_TCHAR, *FString::Printf(Format, ##__VA_ARGS__))

#pragma endregion

#define ECC_PickUp ECollisionChannel::ECC_GameTraceChannel2
