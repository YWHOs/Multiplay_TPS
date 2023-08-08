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
		for (uint32 i = 0; i < pellets; i++)
		{
			FVector end = TraceEnd(start, HitTarget);
		}
	}
}
