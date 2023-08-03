// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FEAnimInstance.h"

#include "GameFramework/Character.h"

void UFEAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(TryGetPawnOwner());
}

void UFEAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if(IsValid(Character))
	{
		CurrentPawnSpeed = Character->GetVelocity().Size();
		IsCrouching = Character->bIsCrouched;
	}
}
