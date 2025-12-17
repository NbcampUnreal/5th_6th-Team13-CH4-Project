#pragma once

#include "CoreMinimal.h"
#include "FCKeyType.generated.h"

UENUM(BlueprintType)
enum class EFCKeyType : uint8 
{
	None		UMETA(DisplayName = "None"),
	Key_Red		UMETA(DisplayName = "Red Key"),
	Key_Blue	UMETA(DisplayName = "Blue Key"),
	Key_Green	UMETA(DisplayName = "Green Key"),

};
