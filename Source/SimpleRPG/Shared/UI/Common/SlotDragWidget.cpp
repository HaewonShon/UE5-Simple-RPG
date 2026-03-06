// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/UI/Common/SlotDragWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void USlotDragWidget::OnDragBegin(UTexture2D* Texture)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Icon->SetBrushFromTexture(Texture);
}

void USlotDragWidget::OnDragEnd()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void USlotDragWidget::SetDesiredSize(FVector2D Size)
{
	SizeBox->SetWidthOverride(Size.X);
	SizeBox->SetHeightOverride(Size.Y);
}