// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString _TextToDisplay)
{
	if (displayText)
	{
		displayText->SetText(FText::FromString(_TextToDisplay));
	}
}
void UOverheadWidget::ShowPlayerNetRole(APawn* _InPawn)
{
	ENetRole remoteRole = _InPawn->GetRemoteRole();
	FString role;
	switch (remoteRole)
	{
	case ENetRole::ROLE_Authority:
		role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		role = FString("None");
		break;
	}
	FString remoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *role);
	SetDisplayText(remoteRoleString);
}
void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
