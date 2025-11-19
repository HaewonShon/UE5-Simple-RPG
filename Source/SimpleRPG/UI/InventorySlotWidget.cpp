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

	ItemImage->SetBrushFromTexture(Item->ItemData->Icon);
	if (Item->StackCount > 1)
	{
		StackText->SetText(FText::AsNumber(Item->StackCount));
	}
}