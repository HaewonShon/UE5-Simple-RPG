// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotWidget.h"
#include "ItemDragDropOp.h"
#include "../ItemData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Shared/UI/UISubsystem.h"
#include "../ItemRarityColorData.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UItemSlotWidget::UpdateSlot(const FSlotContent& NewSlotContent)
{
	Super::UpdateSlot(NewSlotContent);

	if (SlotContent.Quantity > 1)
	{
		QuantityText->SetText(FText::AsNumber(SlotContent.Quantity));
	}
	else
	{
		QuantityText->SetText(FText::GetEmpty());
	}

	// Background color setting
	UUISubsystem* UISubsystem = GetOwningLocalPlayer()->GetSubsystem<UUISubsystem>();
	const UItemRarityColorData* ColorData = UISubsystem->GetItemRarityColorData();
	const UItemData* ItemData = Cast<UItemData>(SlotContent.ContentAsset);
	Background->SetBrushColor(ColorData->GetColorForRarity(ItemData->Rarity));

	this->InvalidateLayoutAndVolatility(); // Refresh InvalidationBox cache
}

void UItemSlotWidget::ClearItem()
{
	Icon->SetBrushFromTexture(nullptr);
	Icon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
	QuantityText->SetText(FText());
	Background->SetBrushColor(DefaultBackgroundColor);
}

void UItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DefaultBackgroundColor = Background->GetBrushColor();
	ClearItem();
}