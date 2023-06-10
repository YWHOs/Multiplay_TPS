// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Multiplay_TPS/Weapon/Weapon.h"
#include "Multiplay_TPS/Components/CombatComponent.h"

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
}
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ATPSCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ATPSCharacter::EquipPressed);
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