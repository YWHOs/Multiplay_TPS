// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPSPlayerController.generated.h"

/**
 * 
 */
class ATPSHUD;
class UCharacterOverlay;
class ATPSGameMode;

UCLASS()
class MULTIPLAY_TPS_API ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHUDHealth(float _Health, float _MaxHealth);
	void SetHUDShield(float _Shield, float _MaxShield);
	void SetHUDScore(float _Score);
	void SetHUDDie(int32 _Die);
	void SetHUDAmmo(int32 _Ammo);
	void SetHUDCarriedAmmo(int32 _Ammo);
	void SetHUDCountdown(float _Count);
	void SetHUDAnnouncement(float _Count);
	void SetHUDGrenade(int32 _Grenade);
	virtual void OnPossess(APawn* _Pawn) override;
	virtual void Tick(float DeltaTime) override;

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName _State);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();

	// 클라이언트 와 서버 싱크 시간
	UFUNCTION(Server, Reliable)
	void ServerRequestTime(float _Time);
	UFUNCTION(Client, Reliable)
	void ClientReportTime(float _Time, float _TimeServerRequest);
	void CheckTimeSync(float _DeltaTime);

	void HandleMatch();
	void HandleCooldown();
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client, Reliable)
	void ClientJoinGame(FName _StateMatch, float _Warmup, float _MatchTime, float _StartTime, float _Cooldown);

protected:
	// 클라이언트와 서버 시간 차이
	float clientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category = Time)
	float timeSyncFrequency = 5.f;
	float timeSyncRunningTime = 0.f;

private:
	UFUNCTION()
	void OnRep_MatchState();
private:
	UPROPERTY()
	ATPSHUD* TPSHUD;
	UPROPERTY()
	ATPSGameMode* TPSGameMode;

	float levelStartTime = 0.f;
	float matchTime = 0.f;
	float warmupTime = 0.f;
	float cooldownTime = 0.f;
	uint32 countdown = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName matchState;

	UPROPERTY()
	UCharacterOverlay* characterOverlay;
	
	float HUDHealth;
	float HUDMaxHealth;
	bool bInitHealth = false;

	float HUDShield;
	float HUDMaxShield;
	bool bInitShield = false;

	float HUDScore;
	bool bInitScore = false;

	int32 HUDDie;
	bool bInitDie = false;

	int32 HUDGrenade;
	bool bInitGrenade = false;

	float HUDCarriedAmmo;
	bool bInitCarriedAmmo = false;
	float HUDAmmo;
	bool bInitAmmo = false;
};
