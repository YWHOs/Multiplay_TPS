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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Die();

	// Score은 PlayerState에 원래 있음으로 float를 사용
	void AddToScore(float _Score);
	void AddToDie(int32 _Die);

private:
	UPROPERTY()
	ATPSCharacter* character;
	UPROPERTY()
	ATPSPlayerController* playerController;

	UPROPERTY(ReplicatedUsing = OnRep_Die)
	int32 die;

	
};
