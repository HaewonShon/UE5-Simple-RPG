// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CommonWidgetData.generated.h"

/**
 *		Widget reference holder for Widget subsystem
 */
UCLASS()
class SIMPLERPG_API UCommonWidgetData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UQuantityConfirmationWidget> QuantityConfirmationWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMessageBox> MessageBoxWidgetClass;
};
