// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class ATPSCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAY_TPS_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuffComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Heal(float _Heal, float _HealTime);
	void Speed(float _BaseSpeed, float _CrouchSpeed, float _BuffTime);
	void Jump(float _Jump, float _BuffTime);
	void SetInitialSpeed(float _BaseSpeed, float _CrouchSpeed);
	void SetInitialJump(float _Jump);
public:
	friend class ATPSCharacter;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void HealRampUp(float _DeltaTime);

private:
	void ResetSpeed();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float _BaseSpeed, float _CrouchSpeed);

	void ResetJump();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float _Jump);
private:
	UPROPERTY()
	ATPSCharacter* character;

	// Heal
	bool bHealing = false;
	float healRate = 0.f;
	float amountToHeal = 0.f;

	// Speed
	FTimerHandle speedBuffTimer;
	float initialBaseSpeed;
	float initialCrouchSpeed;

	// Jump
	FTimerHandle jumpBuffTimer;
	float initialJump;
};
