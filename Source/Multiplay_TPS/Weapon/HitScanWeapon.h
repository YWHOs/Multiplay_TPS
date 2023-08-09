// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
class UParticleSystem;

UCLASS()
class MULTIPLAY_TPS_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

protected:
	FVector TraceEnd(const FVector& _Start, const FVector& _Target);
	void WeaponTraceHit(const FVector& _Start, const FVector& _HitTarget, FHitResult& _OutHit);

protected:
	UPROPERTY(EditAnywhere)
	UParticleSystem* impactParticle;

	UPROPERTY(EditAnywhere)
	USoundCue* hitSound;
	UPROPERTY(EditAnywhere)
	float damage = 20.f;
private:


	UPROPERTY(EditAnywhere)
	UParticleSystem* beamParticle;

	UPROPERTY(EditAnywhere)
	UParticleSystem* muzzleFlash;
	UPROPERTY(EditAnywhere)
	USoundCue* fireSound;


	// ªÍ¡°µµ
	UPROPERTY(EditAnywhere, Category = "Scatter")
	float distanceToSphere = 800.f;
	UPROPERTY(EditAnywhere, Category = "Scatter")
	float sphereRadius = 75.f;
	UPROPERTY(EditAnywhere, Category = "Scatter")
	bool bUseScatter = false;
};
