// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSAnimInstance.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	bWeaponEquipped = character->IsWeaponEquipped();
	bIsCrouched = character->bIsCrouched;
	bAiming = character->IsAiming();

	// 스트레이핑
	FRotator aimRotation = character->GetBaseAimRotation();
	FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(character->GetVelocity());
	FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);
	deltaRotation = FMath::RInterpTo(deltaRotation, deltaRot, DeltaTime, 6.f);
	yawOffset = deltaRotation.Yaw;

	characterRotationLast = characterRotation;
	characterRotation = character->GetActorRotation();
	const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(characterRotation, characterRotationLast);
	const float target = delta.Yaw / DeltaTime;
	const float interp = FMath::FInterpTo(lean, target, DeltaTime, 6.f);
	lean = FMath::Clamp(interp, -90.f, 90.f);

	ao_Yaw = character->GetAO_Yaw();
	ao_Pitch = character->GetAO_Pitch();
}
