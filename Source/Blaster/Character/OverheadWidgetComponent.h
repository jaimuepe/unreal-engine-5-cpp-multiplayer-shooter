// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "OverheadWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverheadWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	void ShowName();

	void HideName();
};
