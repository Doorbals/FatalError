// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FEAnimInstance.h"

#include "GameFramework/Character.h"

void UFEAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if(IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		IsCrouching = Character->bIsCrouched;
	}
}
