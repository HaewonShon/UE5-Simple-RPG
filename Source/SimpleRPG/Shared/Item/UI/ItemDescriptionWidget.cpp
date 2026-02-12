// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDescriptionWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UItemDescriptionWidget::SetDescription(const FItemDescription& Description)
{
	DisplayName->SetText(Description.Name);
	DisplayIcon->SetBrushFromTexture(Description.Icon);

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
}