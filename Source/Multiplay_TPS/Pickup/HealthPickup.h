// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
UCLASS()
class MULTIPLAY_TPS_API AHealthPickup : public APickup
{
	GENERATED_BODY()
	
public:
	AHealthPickup();
	virtual void Destroyed() override;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	float heal = 100.f;
	UPROPERTY(EditAnywhere)
	float healTime = 5.f;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* pickupEffectComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* pickupEffect;
};
