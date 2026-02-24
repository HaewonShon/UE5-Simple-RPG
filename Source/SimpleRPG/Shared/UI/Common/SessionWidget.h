// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnWidgetClosed);

/**
 *   base class for widgets with lifecycle, which supports closing notify for root widget
 */
UCLASS(Abstract)
class SIMPLERPG_API USessionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void CloseWidget();
	FOnWidgetClosed OnWidgetClosed;
};