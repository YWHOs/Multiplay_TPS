// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TPSGameMode.generated.h"

/**
 * 
 */
class ATPSCharacter;
class ATPSPlayerController;

UCLASS()
class MULTIPLAY_TPS_API ATPSGameMode : public AGameMode
{
	GENERATED_BODY()
	

public:
	virtual void PlayerEliminated(ATPSCharacter* _ElimCharacter, ATPSPlayerController* _VictimController, ATPSPlayerController* _AttackController);
	virtual void RequestRespawn(ACharacter* _ElimCharacter, AController* _ElimController);
};
