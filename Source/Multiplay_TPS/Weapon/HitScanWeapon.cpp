// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

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
			FVector beamEnd = end;
			if (fireHit.bBlockingHit)
			{
				beamEnd = fireHit.ImpactPoint;
				ATPSCharacter* character = Cast<ATPSCharacter>(fireHit.GetActor());
				if (character && HasAuthority() && instigatorController)
				{
					UGameplayStatics::ApplyDamage(character, damage, instigatorController, this, UDamageType::StaticClass());
				}
				if (impactParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(world, impactParticle, fireHit.ImpactPoint, fireHit.ImpactNormal.Rotation());
				}
				if (hitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, hitSound, fireHit.ImpactPoint);
				}
			}
			if (beamParticle)
			{
				UParticleSystemComponent* beam = UGameplayStatics::SpawnEmitterAtLocation(world, beamParticle, socketTransform);
				if (beam)
				{
					beam->SetVectorParameter(FName("Target"), beamEnd);
				}
			}
		}
		if (muzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(world, muzzleFlash, socketTransform);
		}
		if (fireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, fireSound, GetActorLocation());
		}
	}
}