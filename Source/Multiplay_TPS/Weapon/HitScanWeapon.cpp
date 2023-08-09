// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"
#include "Kismet/KismetMathLibrary.h"

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

		FHitResult fireHit;
		WeaponTraceHit(start, HitTarget, fireHit);

		ATPSCharacter* character = Cast<ATPSCharacter>(fireHit.GetActor());
		if (character && HasAuthority() && instigatorController)
		{
			UGameplayStatics::ApplyDamage(character, damage, instigatorController, this, UDamageType::StaticClass());
		}
		if (impactParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactParticle, fireHit.ImpactPoint, fireHit.ImpactNormal.Rotation());
		}
		if (hitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, hitSound, fireHit.ImpactPoint);
		}

		if (muzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleFlash, socketTransform);
		}
		if (fireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, fireSound, GetActorLocation());
		}
	}
}
void AHitScanWeapon::WeaponTraceHit(const FVector& _Start, const FVector& _HitTarget, FHitResult& _OutHit)
{
	UWorld* world = GetWorld();
	if (world)
	{
		FVector end = bUseScatter ? TraceEnd(_Start, _HitTarget) : _Start + (_HitTarget - _Start) * 1.25f;

		world->LineTraceSingleByChannel(_OutHit, _Start, end, ECollisionChannel::ECC_Visibility);
		FVector beamEnd = end;
		if (_OutHit.bBlockingHit)
		{
			beamEnd = _OutHit.ImpactPoint;
			if (beamParticle)
			{
				UParticleSystemComponent* beam = UGameplayStatics::SpawnEmitterAtLocation(world, beamParticle, _Start, FRotator::ZeroRotator, true);
				if (beam)
				{
					beam->SetVectorParameter(FName("Target"), beamEnd);
				}
			}
		}
	}
}

FVector AHitScanWeapon::TraceEnd(const FVector& _Start, const FVector& _Target)
{
	FVector toTarget = (_Target - _Start).GetSafeNormal();
	FVector sphereCenter = _Start + toTarget * distanceToSphere;
	FVector randVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, sphereRadius);
	FVector end = sphereCenter + randVec;
	FVector toEnd = end - _Start;

	return FVector(_Start + toEnd * TRACE_LENGTH / toEnd.Size());
}