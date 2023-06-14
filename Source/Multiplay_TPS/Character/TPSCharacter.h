// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Multiplay_TPS/TPSTypes/TurningInPlace.h"
#include "TPSCharacter.generated.h"

UCLASS()
class MULTIPLAY_TPS_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATPSCharacter();

protected:
	virtual void BeginPlay() override;

	void MoveForward(float _value);
	void MoveRight(float _value);
	void Turn(float _value);
	void LookUp(float _value);
	void EquipPressed();
	void CrouchPressed();
	void AimPressed();
	void AimReleased();
	void AimOffset(float DeltaTime);
	virtual void Jump() override;
	void FirePressed();
	void FireReleased();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* cameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* followCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* overheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* overlappingWeapon;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* combatComponent;

	float ao_Yaw;
	float ao_Pitch;
	float interpAO_Yaw;
	FRotator startAimRotation;

	ETurningInPlace turningInPlace;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* fireWeaponMontage;

private:
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* _LastWeapon);

	void TurnInPlace(float DeltaTime);

public:
	void SetOverlappingWeapon(AWeapon* _Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetEquippedWeapon();

public:
	FORCEINLINE float GetAO_Yaw() const { return ao_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return ao_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return turningInPlace; }
};
