// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPSPlayerController.generated.h"

/**
 * 
 */
class ATPSHUD;

UCLASS()
class MULTIPLAY_TPS_API ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHUDHealth(float _Health, float _MaxHealth);
	void SetHUDScore(float _Score);
	void SetHUDDie(int32 _Die);
	virtual void OnPossess(APawn* _Pawn) override;


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ATPSHUD* TPSHUD;

};
