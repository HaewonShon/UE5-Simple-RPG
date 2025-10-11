// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressDisplayWidget.generated.h"

/**
 *	Simple Interface displays current HP rate as a bar
 */
UCLASS(Abstract, Blueprintable)
class SIMPLERPG_API UProgressDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetMaxValue(float MaxValueAmount, bool SetHPFull = true, bool KeepProportion = false);

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentValue(float CurrentValueAmount);

	virtual void NativeTick(const FGeometry& Geometry, float InDeltaTime) override;

	/*
	*	Variables
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ProgressText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> Background;

	UPROPERTY(BlueprintReadWrite)
	bool ShouldAnimate;

	UPROPERTY(BlueprintReadWrite)
	float MaxValue;

	float CurrentValue;

	float DisplayValue;
	 
	bool bNeedUpdate;
};
