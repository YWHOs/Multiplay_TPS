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
#include "Camera/CameraComponent.h"

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

		if (character->GetFollowCamera())
		{
			defaultFOV = character->GetFollowCamera()->FieldOfView;
			currentFOV = defaultFOV;
		}
	}
}
// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (character && character->IsLocallyControlled())
	{
		FHitResult hitResult;
		TraceUnderCrosshairs(hitResult);
		hitTarget = hitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		//Zoom
		InterpFOV(DeltaTime);
	}

}
void UCombatComponent::InterpFOV(float _DeltaTime)
{
	if (equippedWeapon == nullptr) return;

	if (bAiming)
	{
		currentFOV = FMath::FInterpTo(currentFOV, equippedWeapon->GetZoomed(), _DeltaTime, equippedWeapon->GetInterpSpeed());
	}
	else
	{
		currentFOV = FMath::FInterpTo(currentFOV, defaultFOV, _DeltaTime, zoomInterpSpeed);
	}

	if (character && character->GetFollowCamera())
	{
		character->GetFollowCamera()->SetFieldOfView(currentFOV);
	}
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
			// 크로스헤어 늘리기

			FVector2D walkRange(0.f, character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D velocityMultiplierRange(0.f, 1.f);
			FVector velocity = character->GetVelocity();
			velocity.Z = 0.f;

			crosshairVelocity = FMath::GetMappedRangeValueClamped(walkRange, velocityMultiplierRange, velocity.Size());

			float interpSpeed = 30.f;
			// 크로스헤어(점프 시)
			if (character->GetCharacterMovement()->IsFalling())
			{
				crosshairInAir = FMath::FInterpTo(crosshairInAir, 2.25f, _DeltaTime, interpSpeed);
			}
			else
			{
				crosshairInAir = FMath::FInterpTo(crosshairInAir, 0.f, _DeltaTime, interpSpeed);
			}
			// 크로스헤어 (조준 시)
			if (bAiming)
			{
				crosshairAim = FMath::FInterpTo(crosshairAim, 0.6f, _DeltaTime, interpSpeed);
			}
			else
			{
				crosshairAim = FMath::FInterpTo(crosshairAim, 0.f, _DeltaTime, interpSpeed);
			}
			crosshairShooting = FMath::FInterpTo(crosshairShooting, 0.f, _DeltaTime, interpSpeed);
			HUDPackage.crosshairSpread = crosshairVelocity + crosshairInAir - crosshairAim + crosshairShooting + 0.5f;

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

		if (equippedWeapon)
		{
			float shootValue = 0.6f;
			crosshairShooting = shootValue;
		}
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

		if (character)
		{
			float distanceToCharacter = (character->GetActorLocation() - start).Size();
			start += crosshairWorldDirection * (distanceToCharacter + 100.f);
		}

		FVector end = start + crosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(_TraceHitResult, start, end, ECollisionChannel::ECC_Visibility);

		// Actor 발견시 크로스헤어 색 변경
		if (_TraceHitResult.GetActor() && _TraceHitResult.GetActor()->Implements<UInteractCrosshair_Interface>())
		{
			HUDPackage.crosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.crosshairColor = FLinearColor::White;
		}
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