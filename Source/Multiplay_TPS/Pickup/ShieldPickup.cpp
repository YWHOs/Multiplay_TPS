// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldPickup.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Multiplay_TPS/Components/BuffComponent.h"

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OhterComp, OtherBodyIndex, bFromSweep, SweepResult);

	ATPSCharacter* TPSCharacter = Cast<ATPSCharacter>(OtherActor);
	if (TPSCharacter)
	{
		UBuffComponent* buffComponent = TPSCharacter->GetBuffComponent();
		if (buffComponent)
		{
			buffComponent->Shield(shield, shieldTime);
		}
	}
	Destroy();
}