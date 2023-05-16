// Fill out your copyright notice in the Description page of Project Settings.

#include "FEGameplayTags.h"
#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"

FFEGameplayTags FFEGameplayTags::GameplayTags;

// GameplayTags Manager에 모든 Input Tag를 Native Tag로 등록
void FFEGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(GameplayTagsManager);
	GameplayTagsManager.DoneAddingNativeTags();
}

void FFEGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_Move, "Input.Move", "Move input.");
	AddTag(InputTag_Look, "Input.Look", "Look input.");
	AddTag(InputTag_Crouch, "Input.Crouch", "Crouch input.");
	AddTag(InputTag_Jump, "Input.Ability.Jump", "Jump input");
	AddTag(InputTag_Sneak, "Input.Ability.Sneak", "Sneak input.");
	AddTag(InputTag_Assassinate, "Input.Ability.Assassinate", "Assassinate input.");
}

// 입력 받은 Tag를 Manager에 Native Tag로 등록함과 동시에 OutTag에 해당 Tag 저장
void FFEGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
