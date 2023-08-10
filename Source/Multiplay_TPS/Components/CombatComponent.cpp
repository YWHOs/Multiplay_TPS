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
#include "TimerManager.h"
#include "Sound/SoundCue.h"

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
		if (character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}
}
void UCombatComponent::InitializeCarriedAmmo()
{
	carriedAmmoMap.Emplace(EWeaponType::EWT_Rifle, startAmmo);
	carriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, startRocketAmmo);
	carriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, startPistolAmmo);
	carriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, startSMGAmmo);
	carriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, startShotgunAmmo);
	carriedAmmoMap.Emplace(EWeaponType::EWT_Sniper, startSniperAmmo);
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
		equippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (handSocket)
		{
			handSocket->AttachActor(equippedWeapon, character->GetMesh());
		}
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = true;
		if (equippedWeapon->equipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, equippedWeapon->equipSound, character->GetActorLocation());
		}
	}
}
void UCombatComponent::OnRep_CarriedAmmo()
{
	controller = controller == nullptr ? Cast<ATPSPlayerController>(character->Controller) : controller;
	if (controller)
	{
		controller->SetHUDCarriedAmmo(carriedAmmo);
	}
}
void UCombatComponent::StartFireTimer()
{
	if (equippedWeapon == nullptr || character == nullptr) return;
	character->GetWorldTimerManager().SetTimer(fireTimer, this, &UCombatComponent::FireTimerFinish, equippedWeapon->fireDelay);
}
void UCombatComponent::FireTimerFinish()
{
	if (equippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFirePressed && equippedWeapon->bAutomatic)
	{
		Fire();
	}
	if (equippedWeapon->IsAmmoEmpty())
	{
		Reload();
	}
}
void UCombatComponent::FirePressed(bool _bPressed)
{
	bFirePressed = _bPressed;

	if (bFirePressed)
	{
		Fire();
	}
}
void UCombatComponent::Fire()
{
	if (CanFire())
	{
		ServerFire(hitTarget);
		if (equippedWeapon)
		{
			bCanFire = false;
			float shootValue = 0.6f;
			crosshairShooting = shootValue;
		}
		StartFireTimer();
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
	if (character && combatState == ECombatState::ECS_Unoccupied)
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
	DOREPLIFETIME_CONDITION(UCombatComponent, carriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, combatState);
}

void UCombatComponent::EquipWeapon(AWeapon* _WeaponToEquip)
{
	if (character == nullptr || _WeaponToEquip == nullptr) return;
	if (equippedWeapon)
	{
		equippedWeapon->Dropped();
	}
	equippedWeapon = _WeaponToEquip;
	equippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (handSocket)
	{
		handSocket->AttachActor(equippedWeapon, character->GetMesh());
	}
	equippedWeapon->SetOwner(character);
	equippedWeapon->SetHUDAmmo();

	if (carriedAmmoMap.Contains(equippedWeapon->GetWeaponType()))
	{
		carriedAmmo = carriedAmmoMap[equippedWeapon->GetWeaponType()];
	}
	controller = controller == nullptr ? Cast<ATPSPlayerController>(character->Controller) : controller;
	if (controller)
	{
		controller->SetHUDCarriedAmmo(carriedAmmo);
	}
	//사운드
	if (equippedWeapon->equipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, equippedWeapon->equipSound, character->GetActorLocation());
	}
	// 재장전
	if (equippedWeapon->IsAmmoEmpty())
	{
		Reload();
	}

	character->GetCharacterMovement()->bOrientRotationToMovement = false;
	character->bUseControllerRotationYaw = true;
}
void UCombatComponent::Reload()
{
	if (carriedAmmo > 0 && combatState != ECombatState::ECS_Reloading)
	{
		ServerReload();
	}
}
int32 UCombatComponent::AmountToReload()
{
	if (equippedWeapon == nullptr) return 0;
	int32 magRoom = equippedWeapon->GetMagCapacity() - equippedWeapon->GetAmmo();

	if (carriedAmmoMap.Contains(equippedWeapon->GetWeaponType()))
	{
		int32 amountCarried = carriedAmmoMap[equippedWeapon->GetWeaponType()];
		int32 least = FMath::Min(magRoom, amountCarried);
		return FMath::Clamp(magRoom, 0, least);
	}

	return 0;
}
void UCombatComponent::FinishReload()
{
	if (character == nullptr) return;
	if (character->HasAuthority())
	{
		combatState = ECombatState::ECS_Unoccupied;
		UpdateAmmo();
	}
	if (bFirePressed)
	{
		Fire();
	}
}
void UCombatComponent::UpdateAmmo()
{
	if (character == nullptr || equippedWeapon == nullptr) return;
	int32 reloadAmount = AmountToReload();
	if (carriedAmmoMap.Contains(equippedWeapon->GetWeaponType()))
	{
		carriedAmmoMap[equippedWeapon->GetWeaponType()] -= reloadAmount;
		carriedAmmo = carriedAmmoMap[equippedWeapon->GetWeaponType()];
	}
	controller = controller == nullptr ? Cast<ATPSPlayerController>(character->Controller) : controller;
	if (controller)
	{
		controller->SetHUDCarriedAmmo(carriedAmmo);
	}
	equippedWeapon->AddAmmo(-reloadAmount);
}
void UCombatComponent::ServerReload_Implementation()
{
	if (character == nullptr || equippedWeapon == nullptr) return;

	combatState = ECombatState::ECS_Reloading;
	HandleReload();
}
void UCombatComponent::HandleReload()
{
	character->PlayReloadMontage();
}
void UCombatComponent::OnRep_CombatState()
{
	switch (combatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFirePressed)
		{
			Fire();
		}
		break;
	}
}
bool UCombatComponent::CanFire()
{
	if (equippedWeapon == nullptr) return false;
	return !equippedWeapon->IsAmmoEmpty() && bCanFire && combatState == ECombatState::ECS_Unoccupied;
}