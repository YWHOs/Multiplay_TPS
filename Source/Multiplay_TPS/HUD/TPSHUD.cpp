// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSHUD.h"

void ATPSHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D viewSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(viewSize);
		const FVector2D viewportCenter(viewSize.X / 2.f, viewSize.Y / 2.f);

		if (HUDPackage.crosshairCenter)
		{
			DrawCrosshair(HUDPackage.crosshairCenter, viewportCenter);
		}
		if (HUDPackage.crosshairLeft)
		{
			DrawCrosshair(HUDPackage.crosshairLeft, viewportCenter);
		}
		if (HUDPackage.crosshairRight)
		{
			DrawCrosshair(HUDPackage.crosshairRight, viewportCenter);
		}
		if (HUDPackage.crosshairTop)
		{
			DrawCrosshair(HUDPackage.crosshairTop, viewportCenter);
		}
		if (HUDPackage.crosshairBottom)
		{
			DrawCrosshair(HUDPackage.crosshairBottom, viewportCenter);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("text"));
		}
	}
}

void ATPSHUD::DrawCrosshair(UTexture2D* _Texture, FVector2D _ViewportCenter)
{
	const float textureWidth = _Texture->GetSizeX();
	const float textureHeight = _Texture->GetSizeY();
	const FVector2D textureDrawPoint(_ViewportCenter.X - (textureWidth / 2.f), _ViewportCenter.Y - (textureHeight / 2.f));

	DrawTexture(_Texture, textureDrawPoint.X, textureDrawPoint.Y, textureWidth, textureHeight, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);
}