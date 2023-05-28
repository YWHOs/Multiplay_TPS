// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSAnimInstance.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	character = Cast<ATPSCharacter>(TryGetPawnOwner());
}

void UTPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (character == nullptr)
	{
		character = Cast<ATPSCharacter>(TryGetPawnOwner());
	}
	if (character == nullptr) return;

	FVector velocity = character->GetVelocity();
	velocity.Z = 0.f;
	speed = velocity.Size();

	bIsAir = character->GetCharacterMovement()->IsFalling();
	bIsAccelerating = character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
}
