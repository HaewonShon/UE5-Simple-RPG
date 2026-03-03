// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDescriptionWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"

#include "Shared/UI/UISubsystem.h"
#include "Shared/Item/ItemRarityColorData.h"

void UItemDescriptionWidget::SetDescription(const FItemDescription& Description)
{
	DisplayName->SetText(Description.Name);
	DisplayIcon->SetBrushFromTexture(Description.Icon);

	UUISubsystem* UISubsystem = GetOwningLocalPlayer()->GetSubsystem<UUISubsystem>();
	const UItemRarityColorData* ColorData = UISubsystem->GetItemRarityColorData();
	Rarity->SetText(UEnum::GetDisplayValueAsText(Description.Rarity));
	ItemInfoBackground->SetBrushColor(ColorData->GetColorForRarity(Description.Rarity));

	if (const FItemDetail* ItemDetail = Description.Payload.TryGet<FItemDetail>())
	{
		Type->SetText(FText::GetEmpty());
		DetailedText->SetText(ItemDetail->DetailText);
	}
	else if (const FEquipmentDetail* EquipmentDetail = Description.Payload.TryGet<FEquipmentDetail>())
	{
		Type->SetText(EquipmentDetail->TypeText);

		FTextBuilder Builder;
		for (const TPair<FText, FText>& Stat : EquipmentDetail->Stats)
		{
			Builder.AppendLine(
				FText::Format(FText::FromString(TEXT("{0}: {1}")),
					Stat.Key, Stat.Value)
			);
		}
		DetailedText->SetText(Builder.ToText());
	}

	if (!Description.Price.IsEmpty())
	{
		PriceText->SetText(Description.Price);
	}
	else
	{
		PriceText->SetText(FText::GetEmpty());
	}
}

void UItemDescriptionWidget::SetPositionInScreen(FVector2D Pos)
{
	if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Slot))
	{
		OverlaySlot->SetPadding(FMargin(Pos.X, Pos.Y, 0.f, 0.f));
	}
	else if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		CanvasSlot->SetPosition(Pos);
	}
}