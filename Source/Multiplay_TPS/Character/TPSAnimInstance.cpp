// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSAnimInstance.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Multiplay_TPS/Weapon/Weapon.h"
#include "Multiplay_TPS/TPSTypes/CombatState.h"

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
	equippedWeapon = character->GetEquippedWeapon();
	bIsCrouched = character->bIsCrouched;
	bAiming = character->IsAiming();

	turningInPlace = character->GetTurningInPlace();
	bRotateRootBone = character->ShouldRotateRootBone();
	bElimmed = character->IsElimmed();

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

	// 캐릭터 무기 들었을 때, 왼손 위치 이동
	if (bWeaponEquipped && equippedWeapon && equippedWeapon->GetWeaponMesh() && character->GetMesh())
	{
		lefthandTransform = equippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector outPos;
		FRotator outRot;
		character->GetMesh()->TransformToBoneSpace(FName("hand_r"), lefthandTransform.GetLocation(), FRotator::ZeroRotator, outPos, outRot);
		lefthandTransform.SetLocation(outPos);
		lefthandTransform.SetRotation(FQuat(outRot));

		// 총구의 위치와 크로스헤어 중앙의 위치를 Rotate 시키기
		if (character->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform rightHandTransform = equippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(rightHandTransform.GetLocation(), rightHandTransform.GetLocation() + (rightHandTransform.GetLocation() - character->GetHitTarget()));
			rightHandRotation = FMath::RInterpTo(rightHandRotation, lookAtRotation, DeltaTime, 30.f);
		}
	}
	
	bUseFABRIK = character->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffset = character->GetCombatState() != ECombatState::ECS_Reloading && !character->GetDisableGameplay();
	bTransformRightHand = character->GetCombatState() != ECombatState::ECS_Reloading && !character->GetDisableGameplay();
}
