// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TPSHUD.generated.h"


class UTexture2D;
class UCharacterOverlay;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	UTexture2D* crosshairCenter;
	UTexture2D* crosshairLeft;
	UTexture2D* crosshairRight;
	UTexture2D* crosshairTop;
	UTexture2D* crosshairBottom;

	float crosshairSpread;

	FLinearColor crosshairColor;
};
/**
 * 
 */
UCLASS()
class MULTIPLAY_TPS_API ATPSHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;
	void AddCharacterOverlay();
public:
	UPROPERTY(EditAnywhere, Category = "Stat")
	TSubclassOf<class UUserWidget> characterOverlayClass;
	UPROPERTY()
	UCharacterOverlay* characterOverlay;

protected:
	virtual void BeginPlay() override;

private:
	FHUDPackage HUDPackage;

	UPROPERTY(EditAnywhere)
	float spreadMax = 16.f;

private:
	void DrawCrosshair(UTexture2D* _Texture, FVector2D _ViewportCenter, FVector2D _Spread, FLinearColor _CrosshairColor);

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& _Package) { HUDPackage = _Package; }
};
