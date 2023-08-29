// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Multiplay_TPS/Components/BuffComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AHealthPickup::AHealthPickup()
{
	bReplicates = true;
	pickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pickup Effect"));
	pickupEffectComponent->SetupAttachment(RootComponent);
}

void AHealthPickup::Destroyed()
{
	if (pickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, pickupEffect, GetActorLocation(), GetActorRotation());
	}
	Super::Destroyed();
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OhterComp, OtherBodyIndex, bFromSweep, SweepResult);

	ATPSCharacter* TPSCharacter = Cast<ATPSCharacter>(OtherActor);
	if (TPSCharacter)
	{
		UBuffComponent* buffComponent = TPSCharacter->GetBuffComponent();
		if (buffComponent)
		{
			buffComponent->Heal(heal, healTime);
		}
	}
	Destroy();
}