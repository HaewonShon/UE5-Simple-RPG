// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressDisplayWidget.h"
#include "Components/ProgressBar.h"

void UProgressDisplayWidget::SetMaxValue(float MaxValueAmount, bool SetFull, bool KeepProportion)
{
	if (SetFull)
	{
		MaxValue = MaxValueAmount;
		CurrentValue = MaxValue;
	}
	else if (KeepProportion) // if max hp changed, keep current hp proportion
	{
		float Proportion = CurrentValue / MaxValue;
		MaxValue = MaxValueAmount;
		CurrentValue = MaxValue * Proportion;
	}
	else
	{
		MaxValue = MaxValueAmount;
	}

	UpdateCurrentValue(CurrentValue);
}

void UProgressDisplayWidget::UpdateCurrentValue(float CurrentValueAmount)
{
	CurrentValue = CurrentValueAmount;
	if (ShouldAnimate)
	{
		bNeedUpdate = true;
	}
	else
	{
		if (ProgressBar)
		{
			ProgressBar->SetPercent(CurrentValue / MaxValue);
		}
	}
}

void UProgressDisplayWidget::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
	Super::NativeTick(Geometry, InDeltaTime);

	if (!bNeedUpdate)
	{
		return;
	}

	DisplayValue = FMath::FInterpTo(DisplayValue, CurrentValue, InDeltaTime, 8.0f);
	if (ProgressBar)
	{
		ProgressBar->SetPercent(DisplayValue);
	}
}