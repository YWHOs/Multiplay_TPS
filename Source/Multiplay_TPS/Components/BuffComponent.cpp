// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

}
void UBuffComponent::SetInitialSpeed(float _BaseSpeed, float _CrouchSpeed)
{
	initialBaseSpeed = _BaseSpeed;
	initialCrouchSpeed = _CrouchSpeed;
}

// Called every frame
void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
}

void UBuffComponent::Heal(float _Heal, float _HealTime)
{
	bHealing = true;
	healRate = _Heal / _HealTime;
	amountToHeal += _Heal;
}

void UBuffComponent::HealRampUp(float _DeltaTime)
{
	if (!bHealing || character == nullptr || character->IsElimmed()) return;

	const float healFrame = healRate * _DeltaTime;
	character->SetHealth(FMath::Clamp(character->GetHealth() + healFrame, 0.f, character->GetMaxHealth()));
	character->UpdateHUDHealth();
	amountToHeal -= healFrame;

	if (amountToHeal <= 0.f || character->GetHealth() >= character->GetMaxHealth())
	{
		bHealing = false;
		amountToHeal = 0.f;
	}
}

void UBuffComponent::Speed(float _BaseSpeed, float _CrouchSpeed, float _BuffTime)
{
	if (character == nullptr) return;
	character->GetWorldTimerManager().SetTimer(speedBuffTimer, this, &UBuffComponent::ResetSpeed, _BuffTime);
	if (character->GetCharacterMovement())
	{
		character->GetCharacterMovement()->MaxWalkSpeed = _BaseSpeed;
		character->GetCharacterMovement()->MaxWalkSpeedCrouched = _CrouchSpeed;
	}
	MulticastSpeedBuff(_BaseSpeed, _CrouchSpeed);
}
void UBuffComponent::ResetSpeed()
{
	if (character == nullptr || character->GetCharacterMovement() == nullptr) return;

	character->GetCharacterMovement()->MaxWalkSpeed = initialBaseSpeed;
	character->GetCharacterMovement()->MaxWalkSpeedCrouched = initialCrouchSpeed;
	MulticastSpeedBuff(initialBaseSpeed, initialCrouchSpeed);

	character->GetWorldTimerManager().ClearTimer(speedBuffTimer);

}
void UBuffComponent::MulticastSpeedBuff_Implementation(float _BaseSpeed, float _CrouchSpeed)
{
	character->GetCharacterMovement()->MaxWalkSpeed = _BaseSpeed;
	character->GetCharacterMovement()->MaxWalkSpeedCrouched = _CrouchSpeed;
}