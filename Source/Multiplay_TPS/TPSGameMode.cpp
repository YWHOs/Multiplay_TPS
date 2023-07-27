// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameMode.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Multiplay_TPS/PlayerController/TPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void ATPSGameMode::PlayerEliminated(ATPSCharacter* _ElimCharacter, ATPSPlayerController* _VictimController, ATPSPlayerController* _AttackController)
{
	if (_ElimCharacter)
	{
		_ElimCharacter->Elim();
	}
}

void ATPSGameMode::RequestRespawn(ACharacter* _ElimCharacter, AController* _ElimController)
{
	if (_ElimCharacter)
	{
		_ElimCharacter->Reset();
		_ElimCharacter->Destroy();
	}
	if (_ElimController)
	{
		TArray<AActor*> playerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), playerStarts);
		int32 selection = FMath::RandRange(0, playerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(_ElimController, playerStarts[selection]);
	}
}