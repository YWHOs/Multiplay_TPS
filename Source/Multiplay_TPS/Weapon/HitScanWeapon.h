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

private:
	UPROPERTY(EditAnywhere)
	float damage = 20.f;
	UPROPERTY(EditAnywhere)
	UParticleSystem* impactParticle;
	UPROPERTY(EditAnywhere)
	UParticleSystem* beamParticle;

	UPROPERTY(EditAnywhere)
	UParticleSystem* muzzleFlash;
	UPROPERTY(EditAnywhere)
	USoundCue* fireSound;
	UPROPERTY(EditAnywhere)
	USoundCue* hitSound;
};
