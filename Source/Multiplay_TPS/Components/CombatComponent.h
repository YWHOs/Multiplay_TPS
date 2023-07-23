// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Multiplay_TPS/HUD/TPSHUD.h"
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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool _bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool _bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FirePressed(bool _bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& _TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& _TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& _TraceHitResult);

	void SetHUDCrosshairs(float _DeltaTime);

private:
	class ATPSCharacter* character;
	class ATPSPlayerController* controller;
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

private:
	void InterpFOV(float _DeltaTime);

public:	
	FORCEINLINE ATPSCharacter* GetCharacter() { return character; }

		
};
