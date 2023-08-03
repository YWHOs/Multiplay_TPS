// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TPSGameState.generated.h"

/**
 * 
 */

class ATPSPlayerState;
UCLASS()
class MULTIPLAY_TPS_API ATPSGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(ATPSPlayerState* _Player);

public:
	UPROPERTY(Replicated)
	TArray<ATPSPlayerState*> topScorePlayers;

private:
	float topScore = 0.f;
};
