// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class MULTIPLAY_TPS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _HitComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, FVector _NormalImpulse, const FHitResult& _Hit);

	void SpawnTrail();
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void ExplodeDamage();

protected:
	UPROPERTY(EditAnywhere)
	float damage = 10.f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* impactParticle;

	UPROPERTY(EditAnywhere)
	class USoundCue* impactSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* collisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* projectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* trailSystem;

	UPROPERTY()
	class UNiagaraComponent* trailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* projectileMesh;

	UPROPERTY(EditAnywhere)
	float damageInnerRadius = 200.f;
	UPROPERTY(EditAnywhere)
	float damageOuterRadius = 500.f;

private:

	UPROPERTY(EditAnywhere)
	class UParticleSystem* tracer;
	UPROPERTY()
	class UParticleSystemComponent* tracerComponent;

	FTimerHandle destroyTimer;

	UPROPERTY(EditAnywhere)
	float destroyTime = 3.f;

};
