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
	void SetHUDAmmo(int32 _Ammo);
	void SetHUDCarriedAmmo(int32 _Ammo);
	void SetHUDCountdown(float _Count);
	virtual void OnPossess(APawn* _Pawn) override;
	virtual void Tick(float DeltaTime) override;

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();

	// 클라이언트 와 서버 싱크 시간
	UFUNCTION(Server, Reliable)
	void ServerRequestTime(float _Time);
	UFUNCTION(Server, Reliable)
	void ClientReportTime(float _Time, float _TimeServerRequest);
	void CheckTimeSync(float _DeltaTime);
protected:
	// 클라이언트와 서버 시간 차이
	float clientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category = Time)
	float timeSyncFrequency = 5.f;
	float timeSyncRunningTime = 0.f;

private:
	UPROPERTY()
	ATPSHUD* TPSHUD;

	float matchTime = 180.f;
	uint32 countdown = 0;

};
