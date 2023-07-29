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

void ATPSPlayerController::SetHUDScore(float _Score)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->scoreAmountText;
	if (bValid)
	{
		FString scoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(_Score));
		TPSHUD->characterOverlay->scoreAmountText->SetText(FText::FromString(scoreText));
	}
}

void ATPSPlayerController::SetHUDDie(int32 _Die)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->dieAmountText;
	if (bValid)
	{
		FString dieText = FString::Printf(TEXT("%d"), _Die);
		TPSHUD->characterOverlay->dieAmountText->SetText(FText::FromString(dieText));
	}
}