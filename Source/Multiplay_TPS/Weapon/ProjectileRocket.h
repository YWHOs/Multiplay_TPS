// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAY_TPS_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileRocket();

protected:
	virtual void OnHit(UPrimitiveComponent* _HitComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, FVector _NormalImpulse, const FHitResult& _Hit) override;
	//virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		USoundCue* ProjectileLoop;

	UPROPERTY()
		UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
		USoundAttenuation* LoopingSoundAttenuation;

	//UPROPERTY(VisibleAnywhere)
		//class URocketMovementComponent* RocketMovementComponent;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rocketMesh;
};
