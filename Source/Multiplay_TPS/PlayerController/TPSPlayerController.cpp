// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"
#include "Multiplay_TPS/HUD/TPSHUD.h"
#include "Multiplay_TPS/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TPSHUD = Cast<ATPSHUD>(GetHUD());
}
void ATPSPlayerController::OnPossess(APawn* _Pawn)
{
	Super::OnPossess(_Pawn);

	ATPSCharacter* character = Cast<ATPSCharacter>(_Pawn);
	if (character)
	{
		SetHUDHealth(character->GetHealth(), character->GetMaxHealth());
	}
}
void ATPSPlayerController::SetHUDHealth(float _Health, float _MaxHealth)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->healthBar && TPSHUD->characterOverlay->healthText;
	if (bValid)
	{
		const float healthPercent = _Health / _MaxHealth;
		TPSHUD->characterOverlay->healthBar->SetPercent(healthPercent);
		FString healthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(_Health), FMath::CeilToInt(_MaxHealth));
		TPSHUD->characterOverlay->healthText->SetText(FText::FromString(healthText));
	}
}