// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TPSPlayerState.generated.h"

/**
 * 
 */
class ATPSCharacter;
class ATPSPlayerController;

UCLASS()
class MULTIPLAY_TPS_API ATPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void OnRep_Score() override;
	void AddToScore(float _Score);

private:
	ATPSCharacter* character;
	ATPSPlayerController* playerController;
};
