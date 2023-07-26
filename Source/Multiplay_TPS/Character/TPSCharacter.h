// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Multiplay_TPS/TPSTypes/TurningInPlace.h"
#include "Multiplay_TPS/Interface/InteractCrosshair_Interface.h"
#include "TPSCharacter.generated.h"

class UAnimMontage;

UCLASS()
class MULTIPLAY_TPS_API ATPSCharacter : public ACharacter, public IInteractCrosshair_Interface
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
	void CalculateAOPitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FirePressed();
	void FireReleased();
	void PlayHitReactMontage();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();

	virtual void OnRep_ReplicatedMovement() override;

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
	UAnimMontage* fireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* hitReactMontage;

	UPROPERTY(EditAnywhere)
	float cameraThreshold = 200.f;

	float turnThreshold = 0.5f;

	// 부드러운 회전
	FRotator proxyRotationLast;
	FRotator proxyRotation;
	float proxyYaw;
	float lastMovementReplication;

	bool bRotateRootBone;

private:
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* _LastWeapon);

	void TurnInPlace(float DeltaTime);
	void HideCamera();

	float CalculateSpeed();

public:
	void SetOverlappingWeapon(AWeapon* _Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;

public:
	FORCEINLINE float GetAO_Yaw() const { return ao_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return ao_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return turningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return followCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
};
