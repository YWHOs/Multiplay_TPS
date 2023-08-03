// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameMode.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Multiplay_TPS/PlayerController/TPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Multiplay_TPS/PlayerState/TPSPlayerState.h"
#include "Multiplay_TPS/GameState/TPSGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}
ATPSGameMode::ATPSGameMode()
{
	bDelayedStart = true;

}
void ATPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	levelStartTime = GetWorld()->GetTimeSeconds();
}
void ATPSGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		countdownTime = warmupTime - GetWorld()->GetTimeSeconds() + levelStartTime;
		if (countdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		countdownTime = warmupTime + matchTime - GetWorld()->GetTimeSeconds() + levelStartTime;
		if (countdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		countdownTime = cooldownTime + warmupTime + matchTime - GetWorld()->GetTimeSeconds() + levelStartTime;
		if (countdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}
void ATPSGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATPSPlayerController* controller = Cast<ATPSPlayerController>(*It);
		if (controller)
		{
			controller->OnMatchStateSet(MatchState);
		}
	}
	
}
void ATPSGameMode::PlayerEliminated(ATPSCharacter* _ElimCharacter, ATPSPlayerController* _VictimController, ATPSPlayerController* _AttackController)
{
	ATPSPlayerState* attackerState = _AttackController ? Cast<ATPSPlayerState>(_AttackController->PlayerState) : nullptr;
	ATPSPlayerState* victimState = _VictimController ? Cast<ATPSPlayerState>(_VictimController->PlayerState) : nullptr;

	ATPSGameState* gameState = GetGameState<ATPSGameState>();

	if (attackerState && attackerState != victimState && gameState)
	{
		attackerState->AddToScore(1.f);
		gameState->UpdateTopScore(attackerState);
	}
	if (victimState)
	{
		victimState->AddToDie(1);
	}
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