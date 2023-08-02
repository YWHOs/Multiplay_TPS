// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"
#include "Multiplay_TPS/HUD/TPSHUD.h"
#include "Multiplay_TPS/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Multiplay_TPS/Character/TPSCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Multiplay_TPS/TPSGameMode.h"
#include "Multiplay_TPS/HUD/Announcement.h"

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TPSHUD = Cast<ATPSHUD>(GetHUD());
	if (TPSHUD)
	{
		TPSHUD->AddAnnouncement();
	}
}
void ATPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSPlayerController, matchState);
}
void ATPSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}
void ATPSPlayerController::PollInit()
{
	if (characterOverlay == nullptr)
	{
		if (TPSHUD && TPSHUD->characterOverlay)
		{
			characterOverlay = TPSHUD->characterOverlay;
			if (characterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDie(HUDDie);
			}
		}
	}
}
void ATPSPlayerController::CheckTimeSync(float _DeltaTime)
{
	timeSyncRunningTime += _DeltaTime;
	if (IsLocalController() && timeSyncRunningTime > timeSyncFrequency)
	{
		ServerRequestTime(GetWorld()->GetTimeSeconds());
		timeSyncRunningTime = 0.f;
	}
}
void ATPSPlayerController::OnPossess(APawn* _Pawn)
{
	Super::OnPossess(_Pawn);

	ATPSCharacter* character = Cast<ATPSCharacter>(_Pawn);
	if (character)
	{
		SetHUDHealth(character->GetHealth(), character->GetMaxHealth());
	}
}
void ATPSPlayerController::SetHUDHealth(float _Health, float _MaxHealth)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->healthBar && TPSHUD->characterOverlay->healthText;
	if (bValid)
	{
		const float healthPercent = _Health / _MaxHealth;
		TPSHUD->characterOverlay->healthBar->SetPercent(healthPercent);
		FString healthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(_Health), FMath::CeilToInt(_MaxHealth));
		TPSHUD->characterOverlay->healthText->SetText(FText::FromString(healthText));
	}
	else
	{
		bInitCharacterOverlay = true;
		HUDHealth = _Health;
		HUDMaxHealth = _MaxHealth;
	}
}

void ATPSPlayerController::SetHUDScore(float _Score)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->scoreAmountText;
	if (bValid)
	{
		FString scoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(_Score));
		TPSHUD->characterOverlay->scoreAmountText->SetText(FText::FromString(scoreText));
	}
	else
	{
		bInitCharacterOverlay = true;
		HUDScore = _Score;
	}
}

void ATPSPlayerController::SetHUDDie(int32 _Die)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->dieAmountText;
	if (bValid)
	{
		FString dieText = FString::Printf(TEXT("%d"), _Die);
		TPSHUD->characterOverlay->dieAmountText->SetText(FText::FromString(dieText));
	}
	else
	{
		bInitCharacterOverlay = true;
		HUDDie = _Die;
	}
}

void ATPSPlayerController::SetHUDAmmo(int32 _Ammo)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->ammoAmountText;
	if (bValid)
	{
		FString ammoText = FString::Printf(TEXT("%d"), _Ammo);
		TPSHUD->characterOverlay->ammoAmountText->SetText(FText::FromString(ammoText));
	}
}
void ATPSPlayerController::SetHUDCarriedAmmo(int32 _Ammo)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->carriedAmountText;
	if (bValid)
	{
		FString ammoText = FString::Printf(TEXT("%d"), _Ammo);
		TPSHUD->characterOverlay->carriedAmountText->SetText(FText::FromString(ammoText));
	}
}

void ATPSPlayerController::SetHUDCountdown(float _Count)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bValid = TPSHUD && TPSHUD->characterOverlay && TPSHUD->characterOverlay->countdownText;
	if (bValid)
	{
		int32 minutes = FMath::FloorToInt(_Count / 60.f);
		int32 seconds = _Count - minutes * 60;

		FString countdownText = FString::Printf(TEXT("%02d : %02d"), minutes, seconds);
		TPSHUD->characterOverlay->countdownText->SetText(FText::FromString(countdownText));
	}
}

void ATPSPlayerController::SetHUDTime()
{
	uint32 leftSeconds = FMath::CeilToInt(matchTime - GetServerTime());

	if (countdown != leftSeconds)
	{
		SetHUDCountdown(matchTime - GetServerTime());
	}
	countdown = leftSeconds;
}

void ATPSPlayerController::ServerRequestTime_Implementation(float _Time)
{
	float serverTime = GetWorld()->GetTimeSeconds();
	ClientReportTime(_Time, serverTime);
}
void ATPSPlayerController::ClientReportTime_Implementation(float _Time, float _TimeServerRequest)
{
	float roundTripTime = GetWorld()->GetTimeSeconds() - _Time;
	float currentServerTime = _TimeServerRequest + (0.5f * roundTripTime);
	clientServerDelta = currentServerTime - GetWorld()->GetTimeSeconds();
}
float ATPSPlayerController::GetServerTime()
{
	return GetWorld()->GetTimeSeconds() + clientServerDelta;
}
void ATPSPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestTime(GetWorld()->GetTimeSeconds());
	}
}

void ATPSPlayerController::OnMatchStateSet(FName _State)
{
	matchState = _State;

	if (matchState == MatchState::InProgress)
	{
		HandleMatch();
	}
}

void ATPSPlayerController::OnRep_MatchState()
{
	if (matchState == MatchState::InProgress)
	{
		HandleMatch();
	}
}
void ATPSPlayerController::HandleMatch()
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	if (TPSHUD)
	{
		TPSHUD->AddCharacterOverlay();
		if (TPSHUD->announcement)
		{
			TPSHUD->announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}