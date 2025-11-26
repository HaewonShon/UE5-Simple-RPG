// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "../Item/ItemData.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::SetItem(const FItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Set Item image"));

	ItemImage->SetBrushFromTexture(Item->ItemData->Icon);
	ItemImage->SetBrushTintColor(FSlateColor(FColor::White));
	if (Item->StackCount > 1)
	{
		StackText->SetText(FText::AsNumber(Item->StackCount));
	}

	this->InvalidateLayoutAndVolatility();
}

void UInventorySlotWidget::ClearItem()
{
	ItemImage->SetBrushTintColor(FSlateColor(FColor::Transparent));
	StackText->SetText(FText());
}