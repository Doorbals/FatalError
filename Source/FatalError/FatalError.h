// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFEAbilityInputID : uint8
{
	// 0 None
	None			UMETA(DisplayName = "None"),
	// 1 Confirm
	Confirm			UMETA(DisplayName = "Confirm"),
	// 2 Cancel
	Cancel			UMETA(DisplayName = "Cancel"),
	// 3 Jump
	Jump			UMETA(DisplayName = "Jump"),
	// 4 C
	Crouch			UMETA(DisplayName = "Crouch"),
	// 5 RMB
	Sneak			UMETA(DisplayName = "Sneak"),
	// 6 LMB
	Assassinate		UMETA(DisplayName = "Assassinate"),
};

UENUM(BlueprintType)
enum class EFEMovementState : uint8
{
	None			UMETA(DisplayName = "None"),
	Grounded		UMETA(DisplayName = "Grounded"),
	InAir			UMETA(DisplayName = "InAir"),
	Sneak			UMETA(DisplayName = "Sneak"),
	Climb			UMETA(DisplayName = "Climb"),
};
