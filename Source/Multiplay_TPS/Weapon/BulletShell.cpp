// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletShell.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABulletShell::ABulletShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(meshComp);
	meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	meshComp->SetSimulatePhysics(true);
	meshComp->SetEnableGravity(true);
	meshComp->SetNotifyRigidBodyCollision(true);
	ejectionImpulse = 10.f;
}

// Called when the game starts or when spawned
void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	
	meshComp->OnComponentHit.AddDynamic(this, &ABulletShell::OnHit);
	meshComp->AddImpulse(GetActorForwardVector() * ejectionImpulse);
}

UFUNCTION()
void ABulletShell::OnHit(UPrimitiveComponent* _HitComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, FVector _NormalImpulse, const FHitResult& _Hit)
{
	if (shellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, shellSound, GetActorLocation());
	}
	Destroy();
}