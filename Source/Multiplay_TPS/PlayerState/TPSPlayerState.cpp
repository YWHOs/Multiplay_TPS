// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerState.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Multiplay_TPS/PlayerController/TPSPlayerController.h"

void ATPSPlayerState::AddToScore(float _Score)
{
	Score += _Score;
	character = character == nullptr ? Cast<ATPSCharacter>(GetPawn()) : character;
	if (character)
	{
		playerController = playerController == nullptr ? Cast<ATPSPlayerController>(character->Controller) : playerController;
		if (playerController)
		{
			playerController->SetHUDScore(Score);
		}
	}
}

void ATPSPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	character = character == nullptr ? Cast<ATPSCharacter>(GetPawn()) : character;
	if (character)
	{
		playerController = playerController == nullptr ? Cast<ATPSPlayerController>(character->Controller) : playerController;
		if (playerController)
		{
			playerController->SetHUDScore(Score);
		}
	}
}