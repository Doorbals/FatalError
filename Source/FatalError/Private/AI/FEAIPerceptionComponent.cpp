// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FEAIPerceptionComponent.h"

// 자극이 완전히 사라질 때(Expired) 호출되는 함수
void UFEAIPerceptionComponent::HandleExpiredStimulus(FAIStimulus& StimulusStore)
{
	Super::HandleExpiredStimulus(StimulusStore);
	OnStimulusExpired.Broadcast(StimulusStore);
	UE_LOG(LogTemp, Warning, TEXT("expired"));
}
