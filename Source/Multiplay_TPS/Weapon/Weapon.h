// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

class USphereComponent;
class UWidgetComponent;
class UTexture2D;
class ATPSCharacter;
class ATPSPlayerController;
class USoundCue;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MULTIPLAY_TPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(bool _bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	bool IsAmmoEmpty();
	void AddAmmo(int32 _Ammo);

public:
	// Texture Crosshairs
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* crosshairCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* crosshairLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* crosshairRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* crosshairTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* crosshairBottom;

	//Zoom
	UPROPERTY(EditAnywhere)
	float zoomed = 30.f;
	UPROPERTY(EditAnywhere)
	float zoomInterpSpeed = 20.f;

	// 자동사격
	UPROPERTY(EditAnywhere)
	float fireDelay = .15f;
	UPROPERTY(EditAnywhere)
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
	USoundCue* equipSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OhterComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* weaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* areaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState weaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UWidgetComponent* pickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* fireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletShell> bulletClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 ammo;
	UPROPERTY(EditAnywhere)
	int32 magCapacity;

	UPROPERTY()
	ATPSCharacter* TPSCharacter;
	UPROPERTY()
	ATPSPlayerController* TPSPlayerController;
	UPROPERTY(EditAnywhere)
	EWeaponType weaponType;
private:
	UFUNCTION()
	void OnRep_WeaponState();

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();
public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return areaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return weaponMesh; }
	FORCEINLINE float GetZoomed() const { return zoomed; }
	FORCEINLINE float GetInterpSpeed() const { return zoomInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType() const { return weaponType; }
	FORCEINLINE int32 GetAmmo() const { return ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return magCapacity; }
};
