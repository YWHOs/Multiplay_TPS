// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;

UCLASS()
class MULTIPLAY_TPS_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* healthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* healthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* scoreAmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* dieAmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ammoAmountText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* carriedAmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* countdownText;
};
