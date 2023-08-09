// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);

	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn == nullptr) return;
	AController* instigatorController = pawn->GetController();

	const USkeletalMeshSocket* muzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleSocket && instigatorController)
	{
		FTransform socketTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());

		FVector start = socketTransform.GetLocation();

		TMap<ATPSCharacter*, uint32> hitMap;
		for (uint32 i = 0; i < pellets; i++)
		{
			FHitResult fireHit;
			WeaponTraceHit(start, HitTarget, fireHit);

			ATPSCharacter* character = Cast<ATPSCharacter>(fireHit.GetActor());
			if (character && HasAuthority() && instigatorController)
			{
				if (hitMap.Contains(character))
				{
					hitMap[character]++;
				}
				else
				{
					hitMap.Emplace(character, 1);
				}
			}
			if (impactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactParticle, fireHit.ImpactPoint, fireHit.ImpactNormal.Rotation());
			}
			if (hitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, hitSound, fireHit.ImpactPoint, .5f, FMath::FRandRange(-.5f, .5f));
			}
		}
		for (auto hitPair : hitMap)
		{
			if (instigatorController)
			{
				if (hitPair.Key && HasAuthority() && instigatorController)
				{
					UGameplayStatics::ApplyDamage(hitPair.Key, damage * hitPair.Value, instigatorController, this, UDamageType::StaticClass());
				}
			}
		}
	}
}
