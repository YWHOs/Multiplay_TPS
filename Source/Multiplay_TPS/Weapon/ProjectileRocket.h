// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */

class URocketMovementComponent;

UCLASS()
class MULTIPLAY_TPS_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileRocket();
	virtual void Destroyed() override;

protected:
	virtual void OnHit(UPrimitiveComponent* _HitComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, FVector _NormalImpulse, const FHitResult& _Hit) override;
	virtual void BeginPlay() override;

protected:
	
	UPROPERTY(EditAnywhere)
	USoundCue* projectileLoop;

	UPROPERTY()
	UAudioComponent* projectileLoopComponent;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* loopSoundAttenuation;

	UPROPERTY(VisibleAnywhere)
	URocketMovementComponent* movementComponent;


private:


};
