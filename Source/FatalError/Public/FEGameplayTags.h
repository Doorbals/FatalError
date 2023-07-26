// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;
/**
 * 
 */
struct FFEGameplayTags
{
public:
	static const FFEGameplayTags& Get() { return GameplayTags; };
	static void InitializeNativeTags();
	
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_SneakMove;
	FGameplayTag InputTag_ClimbMove;
	FGameplayTag InputTag_Look;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Crouch;
	FGameplayTag InputTag_Sneak;
	FGameplayTag InputTag_Assassinate;
	FGameplayTag InputTag_CarryBody;
	FGameplayTag InputTag_Possess;
	FGameplayTag InputTag_Scan;
	
protected:
	// 모든 Tag들을 GameplayTags Manager에 등록하는 함수.
	void AddAllTags(UGameplayTagsManager& Manager);

	// Gameplay Manager에 하나의 태그를 등록하는 함수. AddAllTags 내부에서 사용.
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FFEGameplayTags GameplayTags;
};
