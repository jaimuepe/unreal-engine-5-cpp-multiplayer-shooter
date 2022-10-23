// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h" 

void UOverheadWidget::SetDisplayText(FText TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(TextToDisplay);
	}
}

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	SetDisplayText(FText::FromString(TextToDisplay));
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	if (InPawn)
	{
		ENetRole LocalRole = InPawn->GetLocalRole();
		const FText Role = UEnum::GetDisplayValueAsText(LocalRole);

		SetDisplayText(Role);
	}
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	if (InPawn && InPawn->GetPlayerState())
	{
		SetDisplayText(InPawn->GetPlayerState()->GetPlayerName());
	}
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}
