// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include "Multiplay_TPS/Components/CombatComponent.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"


void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OhterComp, OtherBodyIndex, bFromSweep, SweepResult);

	ATPSCharacter* TPSCharacter = Cast<ATPSCharacter>(OtherActor);
	if (TPSCharacter)
	{
		UCombatComponent* combatComponent = TPSCharacter->GetCombatComponent();
		if (combatComponent)
		{
			combatComponent->PickupAmmo(weaponType, ammoAmount);
		}
	}
	Destroy();
}