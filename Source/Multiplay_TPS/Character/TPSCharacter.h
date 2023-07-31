// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Multiplay_TPS/TPSTypes/TurningInPlace.h"
#include "Multiplay_TPS/Interface/InteractCrosshair_Interface.h"
#include "Components/TimelineComponent.h"
#include "Multiplay_TPS/TPSTypes/CombatState.h"
#include "TPSCharacter.generated.h"

class UAnimMontage;
class USoundCue;
class ATPSPlayerState;
class ATPSPlayerController;

UCLASS()
class MULTIPLAY_TPS_API ATPSCharacter : public ACharacter, public IInteractCrosshair_Interface
{
	GENERATED_BODY()

public:
	ATPSCharacter();

protected:
	virtual void BeginPlay() override;

	void UpdateHUDHealth();
	void MoveForward(float _value);
	void MoveRight(float _value);
	void Turn(float _value);
	void LookUp(float _value);
	void EquipPressed();
	void CrouchPressed();
	void ReloadPressed();
	void AimPressed();
	void AimReleased();
	void AimOffset(float DeltaTime);
	void CalculateAOPitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FirePressed();
	void FireReleased();
	void PlayHitReactMontage();
	UFUNCTION()
	void ReceiveDamage(AActor* _DamagedActor, float _Damage, const UDamageType* _DamageType, class AController* _InstigatorController, AActor* _DamageCauser);
	void TickInit();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	virtual void OnRep_ReplicatedMovement() override;
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	void SetOverlappingWeapon(AWeapon* _Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;

	virtual void Destroyed() override;

	ECombatState GetCombatState() const;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* cameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* followCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* overheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* overlappingWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* combatComponent;

	float ao_Yaw;
	float ao_Pitch;
	float interpAO_Yaw;
	FRotator startAimRotation;

	ETurningInPlace turningInPlace;

	// Animation
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* fireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* reloadMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* hitReactMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* elimMontage;

	UPROPERTY(EditAnywhere)
	float cameraThreshold = 200.f;
	float turnThreshold = 0.5f;

	// 부드러운 회전
	FRotator proxyRotationLast;
	FRotator proxyRotation;
	float proxyYaw;
	float lastMovementReplication;

	bool bRotateRootBone;

	// Health
	UPROPERTY(EditAnywhere, Category = "Stat")
	float maxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Stat")
	float health = 100.f;

	UPROPERTY()
	ATPSPlayerController* TPSController;
	bool bElimmed = false;

	FTimerHandle elimTimer;
	UPROPERTY(EditDefaultsOnly)
	float elimDelay = 3.f;
	// Effect
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* dissolveTimeline;
	FOnTimelineFloat dissolveTrack;
	UPROPERTY(EditAnywhere)
	UCurveFloat* dissolveCurve;
	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* dynamicInstanceDissolve;
	UPROPERTY(EditAnywhere)
	UMaterialInstance* instanceDissolve;
	UPROPERTY(EditAnywhere)
	UParticleSystem* elimBotEffect;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* elimBotComponent;
	UPROPERTY(EditAnywhere)
	USoundCue* elimBotSound;
	UPROPERTY()
	ATPSPlayerState* TPSPlayerState;

private:
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* _LastWeapon);

	void TurnInPlace(float DeltaTime);
	void HideCamera();

	float CalculateSpeed();

	UFUNCTION()
	void OnRep_Health();

	void ElimTimerFinish();
	UFUNCTION()
	void UpdateDissolve(float _Value);
	void StartDissolve();



public:
	FORCEINLINE float GetAO_Yaw() const { return ao_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return ao_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return turningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return followCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return health; }
	FORCEINLINE float GetMaxHealth() const { return maxHealth; }
};
