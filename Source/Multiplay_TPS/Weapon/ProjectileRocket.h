// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
class UNiagaraSystem;
class UNiagaraComponent;
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
	void DestroyTimerFinished();

protected:

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* trailSystem;

	UPROPERTY()
	UNiagaraComponent* trailSystemComponent;
	
	UPROPERTY(EditAnywhere)
	USoundCue* projectileLoop;

	UPROPERTY()
	UAudioComponent* projectileLoopComponent;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* loopSoundAttenuation;

	UPROPERTY(VisibleAnywhere)
	URocketMovementComponent* movementComponent;


private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rocketMesh;

	FTimerHandle destroyTimer;

	UPROPERTY(EditAnywhere)
	float destroyTime = 3.f;
};
