// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(collisionBox);
	collisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	collisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	collisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	projectileMovementComponent->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (tracer)
	{
		tracerComponent = UGameplayStatics::SpawnEmitterAttached(tracer, collisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}
	if (HasAuthority())
	{
		collisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}
void AProjectile::OnHit(UPrimitiveComponent* _HitComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, FVector _NormalImpulse, const FHitResult& _Hit)
{
	Destroy();
}
// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (impactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactParticle, GetActorTransform());
	}
	if (impactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, impactSound, GetActorLocation());
	}
}