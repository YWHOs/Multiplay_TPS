// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"

AProjectileRocket::AProjectileRocket()
{
	rocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket"));
	rocketMesh->SetupAttachment(RootComponent);
	rocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	movementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComeponent"));
	movementComponent->bRotationFollowsVelocity = true;
	movementComponent->SetIsReplicated(true);
}
void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		collisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}

	if (trailSystem)
	{
		trailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(trailSystem, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
	}
	if (projectileLoop && loopSoundAttenuation)
	{
		projectileLoopComponent = UGameplayStatics::SpawnSoundAttached(projectileLoop, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, false, 1.f, 1.f, 0.f, loopSoundAttenuation, (USoundConcurrency*)nullptr, false);
	}
}
void AProjectileRocket::OnHit(UPrimitiveComponent* _HitComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, FVector _NormalImpulse, const FHitResult& _Hit)
{
	if (_OtherActor == GetOwner())
	{
		return;
	}
	APawn* pawn = GetInstigator();
	if (pawn && HasAuthority())
	{
		AController* controller = pawn->GetController();
		if (controller)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, damage, 10.f, GetActorLocation(), 200.f, 500.f, 1.f, UDamageType::StaticClass(), TArray<AActor*>(), this, controller);
		}
	}

	GetWorldTimerManager().SetTimer(destroyTimer, this, &AProjectileRocket::DestroyTimerFinished, destroyTime);

	if (impactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactParticle, GetActorTransform());
	}
	if (impactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, impactSound, GetActorLocation());
	}
	if (rocketMesh)
	{
		rocketMesh->SetVisibility(false);
	}
	if (collisionBox)
	{
		collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (trailSystemComponent && trailSystemComponent->GetSystemInstance())
	{
		trailSystemComponent->GetSystemInstance()->Deactivate();
	}
	if (projectileLoopComponent && projectileLoopComponent->IsPlaying())
	{
		projectileLoopComponent->Stop();
	}

}

void AProjectileRocket::DestroyTimerFinished()
{
	Destroy();
}

void AProjectileRocket::Destroyed()
{

}