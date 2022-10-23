// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidgetComponent.h"

#include "GameFramework/Character.h" 
#include "GameFramework/PlayerState.h" 

#include "../HUD/OverheadWidget.h"

void UOverheadWidgetComponent::ShowName()
{
	InitWidget();

	UOverheadWidget* OverheadWidget = Cast<UOverheadWidget>(GetUserWidgetObject());

	if (ensureAlways(OverheadWidget))
	{
		SetVisibility(true);

		ACharacter* Character = Cast<ACharacter>(GetOwner());

		if (ensureAlways(Character))
		{
			APlayerState* PlayerState = Character->GetPlayerState();
			if (ensureAlways(PlayerState))
			{
				OverheadWidget->SetDisplayText(PlayerState->GetPlayerName());
			}
		}
	}
	else
	{
		SetVisibility(false);
	}
}

void UOverheadWidgetComponent::HideName()
{
	SetVisibility(false);
}
