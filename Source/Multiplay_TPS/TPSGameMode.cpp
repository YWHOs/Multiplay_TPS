// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameMode.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Multiplay_TPS/PlayerController/TPSPlayerController.h"

void ATPSGameMode::PlayerEliminated(ATPSCharacter* _ElimCharacter, ATPSPlayerController* _VictimController, ATPSPlayerController* _AttackController)
{
	if (_ElimCharacter)
	{
		_ElimCharacter->Elim();
	}
}