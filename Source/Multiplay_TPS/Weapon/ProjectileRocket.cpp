// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	rocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket"));
	rocketMesh->SetupAttachment(RootComponent);
	rocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void AProjectileRocket::OnHit(UPrimitiveComponent* _HitComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, FVector _NormalImpulse, const FHitResult& _Hit)
{
	APawn* pawn = GetInstigator();
	if (pawn)
	{
		AController* controller = pawn->GetController();
		if (controller)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, damage, 10.f, GetActorLocation(), 200.f, 500.f, 1.f, UDamageType::StaticClass(), TArray<AActor*>(), this, controller);
		}
	}

	Super::OnHit(_HitComp, _OtherActor, _OtherComp, _NormalImpulse, _Hit);
}