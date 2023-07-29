// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerState.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Multiplay_TPS/PlayerController/TPSPlayerController.h"
#include "Net/UnrealNetwork.h"

void ATPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//UnrealNetwork
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSPlayerState, die);
}
void ATPSPlayerState::AddToScore(float _Score)
{
	SetScore(GetScore() + _Score);
	character = character == nullptr ? Cast<ATPSCharacter>(GetPawn()) : character;
	if (character)
	{
		playerController = playerController == nullptr ? Cast<ATPSPlayerController>(character->Controller) : playerController;
		if (playerController)
		{
			playerController->SetHUDScore(GetScore());
		}
	}
}
void ATPSPlayerState::AddToDie(int32 _Die)
{
	die += _Die;
	character = character == nullptr ? Cast<ATPSCharacter>(GetPawn()) : character;
	if (character)
	{
		playerController = playerController == nullptr ? Cast<ATPSPlayerController>(character->Controller) : playerController;
		if (playerController)
		{
			playerController->SetHUDDie(die);
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
			playerController->SetHUDScore(GetScore());
		}
	}
}
void ATPSPlayerState::OnRep_Die()
{
	character = character == nullptr ? Cast<ATPSCharacter>(GetPawn()) : character;
	if (character)
	{
		playerController = playerController == nullptr ? Cast<ATPSPlayerController>(character->Controller) : playerController;
		if (playerController)
		{
			playerController->SetHUDDie(die);
		}
	}
}