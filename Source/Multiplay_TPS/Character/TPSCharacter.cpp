// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Multiplay_TPS/Weapon/Weapon.h"
#include "Multiplay_TPS/Components/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TPSAnimInstance.h"

ATPSCharacter::ATPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(GetMesh());
	cameraBoom->TargetArmLength = 600.f;
	cameraBoom->bUsePawnControlRotation = true;

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	followCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	overheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	overheadWidget->SetupAttachment(RootComponent);

	combatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	combatComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 900.f);
	
	turningInPlace = ETurningInPlace::ETIP_NotTurn;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}
void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATPSCharacter, overlappingWeapon, COND_OwnerOnly);
}
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
}
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPSCharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ATPSCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ATPSCharacter::EquipPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSCharacter::CrouchPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPSCharacter::AimPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPSCharacter::AimReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPSCharacter::FireReleased);
}

void ATPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (combatComponent)
	{
		combatComponent->character = this;
	}
}

void ATPSCharacter::MoveForward(float _value)
{
	if (Controller && _value != 0.f)
	{
		const FRotator yawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(direction, _value);
	}
}

void ATPSCharacter::MoveRight(float _value)
{
	if (Controller && _value != 0.f)
	{
		const FRotator yawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(direction, _value);
	}
}

void ATPSCharacter::Turn(float _value)
{
	AddControllerYawInput(_value);
}

void ATPSCharacter::LookUp(float _value)
{
	AddControllerPitchInput(_value);
}
void ATPSCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}
void ATPSCharacter::EquipPressed()
{
	if (combatComponent)
	{
		if (HasAuthority())
		{
			combatComponent->EquipWeapon(overlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
		
	}
}
void ATPSCharacter::CrouchPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}
void ATPSCharacter::AimPressed()
{
	if (combatComponent)
	{
		combatComponent->SetAiming(true);
	}
}
void ATPSCharacter::AimReleased()
{
	if (combatComponent)
	{
		combatComponent->SetAiming(false);
	}
}
void ATPSCharacter::FirePressed()
{
	if (combatComponent)
	{
		combatComponent->FirePressed(true);
	}
}
void ATPSCharacter::FireReleased()
{
	if (combatComponent)
	{
		combatComponent->FirePressed(false);
	}
}
void ATPSCharacter::PlayFireMontage(bool bAiming)
{
	if (combatComponent == nullptr || combatComponent->equippedWeapon == nullptr) return;

	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && fireWeaponMontage)
	{
		animInstance->Montage_Play(fireWeaponMontage);
		FName sectionName;
		sectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		animInstance->Montage_JumpToSection(sectionName);
	}
}
void ATPSCharacter::AimOffset(float DeltaTime)
{
	if (combatComponent && combatComponent->equippedWeapon == nullptr) return;

	FVector velocity = GetVelocity();
	velocity.Z = 0.f;
	float speed = velocity.Size();
	bool bIsAir = GetCharacterMovement()->IsFalling();

	// 멈추거나, 점프X
	if (speed == 0.f && !bIsAir)
	{
		FRotator currentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator deltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, startAimRotation);
		ao_Yaw = deltaAimRotation.Yaw;
		if (turningInPlace == ETurningInPlace::ETIP_NotTurn)
		{
			interpAO_Yaw = ao_Yaw;
		}
		bUseControllerRotationYaw = true;

		TurnInPlace(DeltaTime);
	}
	// 달리거나, 점프
	if (speed > 0.f || bIsAir)
	{
		startAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		ao_Yaw = 0.f;
		bUseControllerRotationYaw = true;

		turningInPlace = ETurningInPlace::ETIP_NotTurn;
	}

	ao_Pitch = GetBaseAimRotation().Pitch;
	if (ao_Pitch > 90.f && !IsLocallyControlled())
	{
		// 270, 360 을 -90, 0
		FVector2D inRange(270.f, 360.f);
		FVector2D outRange(-90.f, 0.f);
		ao_Pitch = FMath::GetMappedRangeValueClamped(inRange, outRange, ao_Pitch);
	}
}
void ATPSCharacter::TurnInPlace(float DeltaTime)
{
	if (ao_Yaw > 90.f)
	{
		turningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (ao_Yaw < -90.f)
	{
		turningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (turningInPlace != ETurningInPlace::ETIP_NotTurn)
	{
		// 몸을 돌리면 그 방향으로 Bone 위치 회전
		interpAO_Yaw = FMath::FInterpTo(interpAO_Yaw, 0.f, DeltaTime, 5.f);
		ao_Yaw = interpAO_Yaw;
		if (FMath::Abs(ao_Yaw) < 15.f)
		{
			turningInPlace = ETurningInPlace::ETIP_NotTurn;
			startAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}
void ATPSCharacter::SetOverlappingWeapon(AWeapon* _Weapon)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(false);
	}
	overlappingWeapon = _Weapon;
	if (IsLocallyControlled())
	{
		if (overlappingWeapon)
		{
			overlappingWeapon->ShowPickupWidget(true);
		}
	}
}
void ATPSCharacter::ServerEquipButtonPressed_Implementation()
{
	if (combatComponent)
	{
		combatComponent->EquipWeapon(overlappingWeapon);
	}
}
void ATPSCharacter::OnRep_OverlappingWeapon(AWeapon* _LastWeapon)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(true);
	}
	if (_LastWeapon)
	{
		_LastWeapon->ShowPickupWidget(false);
	}
}
bool ATPSCharacter::IsWeaponEquipped()
{
	return (combatComponent && combatComponent->equippedWeapon);
}
bool ATPSCharacter::IsAiming()
{
	return (combatComponent && combatComponent->bAiming);
}
AWeapon* ATPSCharacter::GetEquippedWeapon()
{
	if (combatComponent == nullptr) return nullptr;
	return combatComponent->equippedWeapon;
}

FVector ATPSCharacter::GetHitTarget() const
{
	if (combatComponent == nullptr) return FVector();
	return combatComponent->hitTarget;
}