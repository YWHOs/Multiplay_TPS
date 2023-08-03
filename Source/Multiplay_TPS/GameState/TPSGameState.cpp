// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameState.h"
#include "Net/UnrealNetwork.h"
#include "Multiplay_TPS/PlayerState/TPSPlayerState.h"

void ATPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSGameState, topScorePlayers);
}

void ATPSGameState::UpdateTopScore(ATPSPlayerState* _Player)
{
	if (topScorePlayers.Num() == 0)
	{
		topScorePlayers.Add(_Player);
		topScore = _Player->GetScore();
	}
	else if (_Player->GetScore() == topScore)
	{
		topScorePlayers.AddUnique(_Player);
	}
	else if (_Player->GetScore() > topScore)
	{
		topScorePlayers.Empty();
		topScorePlayers.AddUnique(_Player);
		topScore = _Player->GetScore();
	}
}