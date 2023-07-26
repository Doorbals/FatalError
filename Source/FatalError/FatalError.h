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
	// 7 E
	CarryBody		UMETA(DisplayName = "CarryBody"),
	// 8 E
	Possess			UMETA(DisplayName = "Possess"),
	// 9 F
	Scan			UMETA(DisplayName = "Scan")
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

UENUM(BlueprintType)
enum class EFEDetectState : uint8
{
	None,
	Green,
	Yellow,
	Orange,
	Red
};

UENUM(BlueprintType)
enum class EFEAIAnimState : uint8
{
	Idle,
	RifleIdle,
	Walk,
	CrouchWalk,
	Run,
	Turn,
	TurnToRun,
	SuddenTurn,
	TurnBeforeRun,
	React,
	LookAround,
	Seek,
	Afraid,
	PushAlert,
	EquipWeapon,
	UnequipWeapon,
	MeleeAttack,
	DistanceAttack,
	Death
};
 