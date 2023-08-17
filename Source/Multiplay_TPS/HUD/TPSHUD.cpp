// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSHUD.h"
#include "CharacterOverlay.h"
#include "Announcement.h"

void ATPSHUD::BeginPlay()
{
	Super::BeginPlay();
}
void ATPSHUD::AddCharacterOverlay()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && characterOverlayClass)
	{
		characterOverlay = CreateWidget<UCharacterOverlay>(playerController, characterOverlayClass);
		characterOverlay->AddToViewport();
	}
}
void ATPSHUD::AddAnnouncement()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && announcementClass)
	{
		announcement = CreateWidget<UAnnouncement>(playerController, announcementClass);
		announcement->AddToViewport();
	}
}
void ATPSHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D viewSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(viewSize);
		const FVector2D viewportCenter(viewSize.X / 2.f, viewSize.Y / 2.f);

		float spreadScaled = spreadMax * HUDPackage.crosshairSpread;

		if (HUDPackage.crosshairCenter)
		{
			FVector2D spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.crosshairCenter, viewportCenter, spread, HUDPackage.crosshairColor);
		}
		if (HUDPackage.crosshairLeft)
		{
			FVector2D spread(-spreadScaled, 0.f);
			DrawCrosshair(HUDPackage.crosshairLeft, viewportCenter, spread, HUDPackage.crosshairColor);
		}
		if (HUDPackage.crosshairRight)
		{
			FVector2D spread(spreadScaled, 0.f);
			DrawCrosshair(HUDPackage.crosshairRight, viewportCenter, spread, HUDPackage.crosshairColor);
		}
		if (HUDPackage.crosshairTop)
		{
			FVector2D spread(0.f, -spreadScaled);
			DrawCrosshair(HUDPackage.crosshairTop, viewportCenter, spread, HUDPackage.crosshairColor);
		}
		if (HUDPackage.crosshairBottom)
		{
			FVector2D spread(0.f, spreadScaled);
			DrawCrosshair(HUDPackage.crosshairBottom, viewportCenter, spread, HUDPackage.crosshairColor);
		}
	}
}

void ATPSHUD::DrawCrosshair(UTexture2D* _Texture, FVector2D _ViewportCenter, FVector2D _Spread, FLinearColor _CrosshairColor)
{
	const float textureWidth = _Texture->GetSizeX();
	const float textureHeight = _Texture->GetSizeY();
	const FVector2D textureDrawPoint(_ViewportCenter.X - (textureWidth / 2.f) + _Spread.X, _ViewportCenter.Y - (textureHeight / 2.f) + _Spread.Y);

	DrawTexture(_Texture, textureDrawPoint.X, textureDrawPoint.Y, textureWidth, textureHeight, 0.f, 0.f, 1.f, 1.f, _CrosshairColor);
}