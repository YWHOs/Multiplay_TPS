// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAY_TPS_API ASpeedPickup : public APickup
{
	GENERATED_BODY()
	

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	float baseSpeedBuff = 1600.f;
	UPROPERTY(EditAnywhere)
	float crouchSpeedBuff = 800.f;
	UPROPERTY(EditAnywhere)
	float speedBuffTime = 10.f;
};
