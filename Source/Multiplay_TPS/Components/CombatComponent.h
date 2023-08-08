// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Multiplay_TPS/HUD/TPSHUD.h"
#include "Multiplay_TPS/Weapon/WeaponTypes.h"
#include "Multiplay_TPS/TPSTypes/CombatState.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f;

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAY_TPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	friend class ATPSCharacter;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* _WeaponToEquip);
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReload();

	void FirePressed(bool _bPressed);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool _bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool _bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& _TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& _TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& _TraceHitResult);
	void SetHUDCrosshairs(float _DeltaTime);
	UFUNCTION(Server, Reliable)
	void ServerReload();
	void HandleReload();
	int32 AmountToReload();

private:
	UPROPERTY()
	class ATPSCharacter* character;
	UPROPERTY()
	class ATPSPlayerController* controller;
	UPROPERTY()
	class ATPSHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* equippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float baseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float aimWalkSpeed;

	bool bFirePressed;

	//crosshair
	float crosshairVelocity;
	float crosshairInAir;
	float crosshairAim;
	float crosshairShooting;

	FVector hitTarget;

	FHUDPackage HUDPackage;

	//Aim
	float defaultFOV;
	UPROPERTY(EditAnywhere)
	float zoomFOV = 30.f;
	float currentFOV;
	UPROPERTY(EditAnywhere)
	float zoomInterpSpeed = 20.f;

	// �ڵ����
	FTimerHandle fireTimer;
	UPROPERTY(VisibleAnywhere)
	bool bCanFire = true;

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 carriedAmmo;
	TMap<EWeaponType, int32> carriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 startAmmo = 30;
	UPROPERTY(EditAnywhere)
	int32 startRocketAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 startPistolAmmo = 0;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState combatState = ECombatState::ECS_Unoccupied;


private:
	void InterpFOV(float _DeltaTime);

	void StartFireTimer();
	void FireTimerFinish();
	bool CanFire();

	UFUNCTION()
	void OnRep_CarriedAmmo();
	void InitializeCarriedAmmo();

	UFUNCTION()
	void OnRep_CombatState();
	void UpdateAmmo();

public:	
	FORCEINLINE ATPSCharacter* GetCharacter() { return character; }

		
};
