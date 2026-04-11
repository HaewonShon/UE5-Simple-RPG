// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Enhancement/UI/EnhanceRequirementSlotWidget.h"
#include "Components/TextBlock.h"

void UEnhanceRequirementSlotWidget::UpdateSlot(const FSlotContent& NewSlotContent)
{
	Super::UpdateSlot(NewSlotContent); // Set Icon
}

void UEnhanceRequirementSlotWidget::SetAmount(int32 OwningAmount, int32 RequiredAmount, bool bIsSufficient)
{
	OwningAmountText->SetText(FText::AsNumber(OwningAmount));
	RequiredAmountText->SetText(FText::AsNumber(RequiredAmount));

	if (!bIsSufficient)
	{
		OwningAmountText->SetColorAndOpacity(FSlateColor(InsufficientDisplayColor));
	}
}

void UEnhanceRequirementSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}