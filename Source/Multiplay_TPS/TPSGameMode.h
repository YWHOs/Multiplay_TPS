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
	ATPSGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(ATPSCharacter* _ElimCharacter, ATPSPlayerController* _VictimController, ATPSPlayerController* _AttackController);
	virtual void RequestRespawn(ACharacter* _ElimCharacter, AController* _ElimController);

public:
	UPROPERTY(EditDefaultsOnly)
	float warmupTime = 10.f;
	float levelStartTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float countdownTime = 0.f;
};
