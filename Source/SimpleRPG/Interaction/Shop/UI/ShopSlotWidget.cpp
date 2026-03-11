// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Shop/UI/ShopSlotWidget.h"
#include "ShopSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UShopSlotWidget::UpdateSlot(const FSlotContent& NewSlotContent)
{
	Super::UpdateSlot(NewSlotContent);
	NameText->SetText(NewSlotContent.ContentAsset->DisplayName);
	QuantityText->SetText(FText::Format(
		FText::FromString(TEXT("Stock: {0}")), NewSlotContent.Quantity));
	
	// Soldout
	if (NewSlotContent.Quantity == 0)
	{
		SoldOutDisplayImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SoldOutDisplayImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UShopSlotWidget::SetPrice(int32 Price)
{
	PriceText->SetText(FText::Format(
		FText::FromString(TEXT("{0} G")), FText::AsCurrency(Price)));
}

void UShopSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}