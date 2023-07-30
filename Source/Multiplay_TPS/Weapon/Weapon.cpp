// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "BulletShell.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Multiplay_TPS/PlayerController/TPSPlayerController.h"

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
	DOREPLIFETIME(AWeapon, ammo);
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
		weaponMesh->SetSimulatePhysics(false);
		weaponMesh->SetEnableGravity(false);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		weaponMesh->SetSimulatePhysics(true);
		weaponMesh->SetEnableGravity(true);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}

}
void AWeapon::OnRep_WeaponState()
{
	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
			ShowPickupWidget(false);
			weaponMesh->SetSimulatePhysics(false);
			weaponMesh->SetEnableGravity(false);
			weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
	case EWeaponState::EWS_Dropped:
		weaponMesh->SetSimulatePhysics(true);
		weaponMesh->SetEnableGravity(true);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}
void AWeapon::SetHUDAmmo()
{
	TPSCharacter = TPSCharacter == nullptr ? Cast<ATPSCharacter>(GetOwner()) : TPSCharacter;
	if (TPSCharacter)
	{
		TPSPlayerController = TPSPlayerController == nullptr ? Cast<ATPSPlayerController>(TPSCharacter->Controller) : TPSPlayerController;
		if (TPSPlayerController)
		{
			TPSPlayerController->SetHUDAmmo(ammo);
		}
	}
}
void AWeapon::OnRep_Ammo()
{
	SetHUDAmmo();
}
void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		TPSCharacter = nullptr;
		TPSPlayerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}

}
void AWeapon::SpendRound()
{
	--ammo;
	SetHUDAmmo();
}
void AWeapon::ShowPickupWidget(bool _bShowWidget)
{
	if (pickupWidget)
	{
		pickupWidget->SetVisibility(_bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (fireAnimation)
	{
		weaponMesh->PlayAnimation(fireAnimation, false);
	}
	if (bulletClass)
	{
		const USkeletalMeshSocket* ammoEjectSocket = weaponMesh->GetSocketByName(FName("AmmoEject"));
		if (ammoEjectSocket)
		{
			FTransform socketTransform = ammoEjectSocket->GetSocketTransform(GetWeaponMesh());

			UWorld* world = GetWorld();
			if (world)
			{
				world->SpawnActor<ABulletShell>(bulletClass, socketTransform.GetLocation(), socketTransform.GetRotation().Rotator());
			}
		}
	}
	SpendRound();
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules detachRules(EDetachmentRule::KeepWorld, true);
	weaponMesh->DetachFromComponent(detachRules);
	SetOwner(nullptr);
	TPSCharacter = nullptr;
	TPSPlayerController = nullptr;
}