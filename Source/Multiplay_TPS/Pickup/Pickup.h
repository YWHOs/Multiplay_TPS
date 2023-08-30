// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class USphereComponent;
class USoundCue;
class UNiagaraComponent;
class UNiagaraSystem;
UCLASS()
class MULTIPLAY_TPS_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere)
	float baseTurnRate = 45.f;

private:
	void BindOverlapTimerFinish();

private:
	UPROPERTY(EditAnywhere)
	USphereComponent* overlapSphere;
	UPROPERTY(EditAnywhere)
	USoundCue* pickupSound;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* pickupMesh;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* pickupEffectComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* pickupEffect;

	FTimerHandle bindOverlapTimer;
	float bindOverlapTime = 0.3f;
	
public:	


};
