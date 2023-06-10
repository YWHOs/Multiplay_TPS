// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(weaponMesh);

	weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	areaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	areaSphere->SetupAttachment(RootComponent);
	areaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	pickupWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		areaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		areaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		areaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	if (pickupWidget)
	{
		pickupWidget->SetVisibility(false);
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, weaponState);
}
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATPSCharacter* character = Cast<ATPSCharacter>(OtherActor);
	if (character)
	{
		character->SetOverlappingWeapon(this);
	}
}
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex)
{
	ATPSCharacter* character = Cast<ATPSCharacter>(OtherActor);
	if (character)
	{
		character->SetOverlappingWeapon(nullptr);
	}
}
void AWeapon::SetWeaponState(EWeaponState State)
{
	weaponState = State;
	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}

}
void AWeapon::OnRep_WeaponState()
{
	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
			ShowPickupWidget(false);
			break;
	}
}
void AWeapon::ShowPickupWidget(bool _bShowWidget)
{
	if (pickupWidget)
	{
		pickupWidget->SetVisibility(_bShowWidget);
	}
}