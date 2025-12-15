// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotDragWidget.h"
#include "Components/Image.h"

void UInventorySlotDragWidget::OnDragBegin(UTexture2D* Texture)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ItemIcon->SetBrushFromTexture(Texture);
}

void UInventorySlotDragWidget::OnDragEnd()
{
	SetVisibility(ESlateVisibility::Collapsed);
}