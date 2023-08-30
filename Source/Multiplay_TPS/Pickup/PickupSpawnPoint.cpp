// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawnPoint.h"
#include "Pickup.h"

APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnPickupTimerStart((AActor*)nullptr);
}

void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupSpawnPoint::SpawnPickup()
{
	int32 numPickupClasses = pickupClasses.Num();
	if (numPickupClasses > 0)
	{
		int32 random = FMath::RandRange(0, numPickupClasses - 1);
		spawnPickup = GetWorld()->SpawnActor<APickup>(pickupClasses[random], GetActorTransform());

		if (HasAuthority() && spawnPickup)
		{
			spawnPickup->OnDestroyed.AddDynamic(this, &APickupSpawnPoint::SpawnPickupTimerStart);
		}
	}
}
void APickupSpawnPoint::SpawnPickupTimerStart(AActor* _DestroyedActor)
{
	const float spawnTime = FMath::RandRange(spawnPickupTimeMin, spawnPickupTimeMax);
	GetWorldTimerManager().SetTimer(spawnPickupTimer, this, &APickupSpawnPoint::SpawnPickupTimerFinish, spawnTime);
}
void APickupSpawnPoint::SpawnPickupTimerFinish()
{
	if (HasAuthority())
	{
		SpawnPickup();
	}
}