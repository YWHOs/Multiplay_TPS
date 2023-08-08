// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Kismet/GameplayStatics.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn == nullptr) return;
	AController* instigatorController = pawn->GetController();

	const USkeletalMeshSocket* muzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleSocket && instigatorController)
	{
		FTransform socketTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());

		FVector start = socketTransform.GetLocation();
		FVector end = start + (HitTarget - start) * 1.25f;

		FHitResult fireHit;
		UWorld* world = GetWorld();
		if (world)
		{
			world->LineTraceSingleByChannel(fireHit, start, end, ECollisionChannel::ECC_Visibility);
			if (fireHit.bBlockingHit)
			{
				ATPSCharacter* character = Cast<ATPSCharacter>(fireHit.GetActor());
				if (character)
				{
					if (HasAuthority())
					{
						UGameplayStatics::ApplyDamage(character, damage, instigatorController, this, UDamageType::StaticClass());
					}
				}
				if (impactParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(world, impactParticle, fireHit.ImpactPoint, fireHit.ImpactNormal.Rotation());
				}
			}
		}
	}
}