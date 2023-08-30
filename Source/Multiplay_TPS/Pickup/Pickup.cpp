// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "Multiplay_TPS/Weapon/WeaponTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	overlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Overlap Sphere"));
	overlapSphere->SetupAttachment(RootComponent);
	overlapSphere->SetSphereRadius(150.f);
	overlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	overlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	overlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	pickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	pickupMesh->SetupAttachment(overlapSphere);
	pickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pickupMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
	pickupMesh->SetRenderCustomDepth(true);
	pickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);

	pickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pickup Effect"));
	pickupEffectComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(bindOverlapTimer, this, &APickup::BindOverlapTimerFinish, bindOverlapTime);
	}

}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pickupMesh)
	{
		pickupMesh->AddWorldRotation(FRotator(0.f, baseTurnRate * DeltaTime, 0.f));
	}
}

void APickup::Destroyed()
{
	Super::Destroyed();

	if (pickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, pickupSound, GetActorLocation());
	}
	if (pickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, pickupEffect, GetActorLocation(), GetActorRotation());
	}
}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void APickup::BindOverlapTimerFinish()
{
	// Pickup, 캐릭터와 겹쳤을 때 생성 가능
	overlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);
}