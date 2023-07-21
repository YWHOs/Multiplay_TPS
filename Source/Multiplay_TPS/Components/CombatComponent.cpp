// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Multiplay_TPS/Weapon/Weapon.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplay_TPS/PlayerController/TPSPlayerController.h"
#include "Multiplay_TPS/HUD/TPSHUD.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	baseWalkSpeed = 600.f;
	aimWalkSpeed = 450.f;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
	}
}
// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetHUDCrosshairs(DeltaTime);
}

void UCombatComponent::SetHUDCrosshairs(float _DeltaTime)
{
	if (character == nullptr || character->Controller == nullptr) return;

	controller = controller == nullptr ? Cast<ATPSPlayerController>(character->Controller) : controller;

	if (controller)
	{
		HUD = HUD == nullptr ? Cast<ATPSHUD>(controller->GetHUD()) : HUD;
		if (HUD)
		{
			FHUDPackage HUDPackage;
			if (equippedWeapon)
			{
				HUDPackage.crosshairCenter = equippedWeapon->crosshairCenter;
				HUDPackage.crosshairLeft = equippedWeapon->crosshairLeft;
				HUDPackage.crosshairRight = equippedWeapon->crosshairRight;
				HUDPackage.crosshairTop = equippedWeapon->crosshairTop;
				HUDPackage.crosshairBottom = equippedWeapon->crosshairBottom;
			}
			else
			{
				HUDPackage.crosshairCenter = nullptr;
				HUDPackage.crosshairLeft = nullptr;
				HUDPackage.crosshairRight = nullptr;
				HUDPackage.crosshairTop = nullptr;
				HUDPackage.crosshairBottom = nullptr;
			}
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::SetAiming(bool _bIsAiming)
{
	bAiming = _bIsAiming;
	ServerSetAiming(_bIsAiming);
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = _bIsAiming ? aimWalkSpeed : baseWalkSpeed;
	}
}
void UCombatComponent::ServerSetAiming_Implementation(bool _bIsAiming)
{
	bAiming = _bIsAiming;
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = _bIsAiming ? aimWalkSpeed : baseWalkSpeed;
	}
}
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (equippedWeapon && character)
	{
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FirePressed(bool _bPressed)
{
	bFirePressed = _bPressed;

	if (bFirePressed)
	{
		FHitResult hitResult;
		TraceUnderCrosshairs(hitResult);
		ServerFire(hitResult.ImpactPoint);
	}
}
void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& _TraceHitTarget)
{
	MulticastFire(_TraceHitTarget);
}
void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& _TraceHitTarget)
{
	// 모든 클라이언트에서 사용가능
	if (equippedWeapon == nullptr) return;
	if (character)
	{
		character->PlayFireMontage(bAiming);
		equippedWeapon->Fire(_TraceHitTarget);
	}
}
void UCombatComponent::TraceUnderCrosshairs(FHitResult& _TraceHitResult)
{
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}

	FVector2D crosshairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);
	FVector crosshairWorldPos;
	FVector crosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), crosshairLocation, crosshairWorldPos, crosshairWorldDirection);

	if (bScreenToWorld)
	{
		FVector start = crosshairWorldPos;

		FVector end = start + crosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(_TraceHitResult, start, end, ECollisionChannel::ECC_Visibility);

		//if (!TraceHitResult.bBlockingHit)
		//{
		//	TraceHitResult.ImpactPoint = end;
		//	hitTarget = end;
		//}
		//else
		//{
		//	hitTarget = TraceHitResult.ImpactPoint;
		//}
	}


}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, equippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::EquipWeapon(AWeapon* _WeaponToEquip)
{
	if (character == nullptr || _WeaponToEquip == nullptr) return;

	equippedWeapon = _WeaponToEquip;
	equippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (handSocket)
	{
		handSocket->AttachActor(equippedWeapon, character->GetMesh());
	}
	equippedWeapon->SetOwner(character);
	character->GetCharacterMovement()->bOrientRotationToMovement = false;
	character->bUseControllerRotationYaw = true;
}