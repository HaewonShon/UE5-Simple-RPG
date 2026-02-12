// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotDragWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UItemSlotDragWidget::OnDragBegin(UTexture2D* Texture)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ItemIcon->SetBrushFromTexture(Texture);
}

void UItemSlotDragWidget::OnDragEnd()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UItemSlotDragWidget::SetDesiredSize(FVector2D Size)
{
	SizeBox->SetWidthOverride(Size.X);
	SizeBox->SetHeightOverride(Size.Y);
}