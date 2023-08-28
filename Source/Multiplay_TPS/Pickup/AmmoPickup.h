// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "Multiplay_TPS/Weapon/WeaponTypes.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAY_TPS_API AAmmoPickup : public APickup
{
	GENERATED_BODY()
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	int32 ammoAmount = 30;

	UPROPERTY(EditAnywhere)
	EWeaponType weaponType;
};
