// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;

	APawn* instigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* muzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (muzzleSocket)
	{
		FTransform socketTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector toTarget = HitTarget - socketTransform.GetLocation();
		FRotator targetRotation = toTarget.Rotation();
		if (projectileClass && instigatorPawn)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = GetOwner();
			spawnParams.Instigator = instigatorPawn;
			UWorld* world = GetWorld();
			if (world)
			{
				world->SpawnActor<AProjectile>(projectileClass, socketTransform.GetLocation(), targetRotation, spawnParams);
			}
		}

	}
}