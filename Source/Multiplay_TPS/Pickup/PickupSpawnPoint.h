// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class MULTIPLAY_TPS_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	void SpawnPickup();
	void SpawnPickupTimerFinish();
	UFUNCTION()
	void SpawnPickupTimerStart(AActor* _DestroyedActor);

protected:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> pickupClasses;
	UPROPERTY()
	APickup* spawnPickup;

private:
	FTimerHandle spawnPickupTimer;

	UPROPERTY(EditAnywhere)
	float spawnPickupTimeMin;
	UPROPERTY(EditAnywhere)
	float spawnPickupTimeMax;
};
